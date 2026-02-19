# 1. Buscar Binario:

Listamos los archivos para que muestre el binario del `level7` y sus permisos:
```bash
level8@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level8 level8   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level8 level8  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level8 level8 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level9 users  6057 Mar  6  2016 level8
-rw-r--r--+ 1 level8 level8   65 Sep 23  2015 .pass
-rw-r--r--  1 level8 level8  675 Apr  3  2012 .profile
```
> **Nota:** El bit `s` en los permisos indica que es un binairo **SUID**. Se ejecuta con los privilegios del propietario (`level9`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level8@RainFall:~$ file ./level8 
./level8: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x3067a180acabc94d328ab89f0a5a914688bf67ab, not stripped
```

### b). ASLR:
```bash
level8@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level8@RainFall:~$ readelf -l ./level8 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level8@RainFall:~$ nm ./level8 
bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level8@RainFall:~$ readelf -S ./level8 | grep -E ".got|.plt"
  [10] .rel.plt          REL             08048374 000374 000050 08   A  5  12  4
  [12] .plt              PROGBITS        08048400 000400 0000b0 04  AX  0   0 16
  [22] .got              PROGBITS        08049a28 000a28 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049a2c 000a2c 000034 04  WA  0   0  4
```
---

# 3. Resumen del análisis:

| Protección | Estado | Evidencia | Consecuencias |
| --- | --- | --- | --- |
| **PIE** | **OFF** | `executable` | El binario siempre carga en `0x08048000` |
| **ASLR** | **OFF** | `0` | Las direcciones de librerías y stack son constantes. |
| **NX** | **OFF** | `GNU_STACK RWE` | en este reto **no** estamos usando Shellcode. |
| **RELRO** | **OFF** | `No RELRO ` | La tabla GOT puede ser sobrescrita. |
| **CANARY** | **? ? ?**  | `error` | Buscar con `gdb` |
| **.got.plt** | **WA**  | `WA` | Confirma sección `.got` en escritura |

---

# 4. Fuzzing:

1. Si ejecutamos el binario sin argumentos, con uno o varios argumentos siempre nos muestra dos `(nil), (nil) ` y deja el terminal abierto:
```bash
level8@RainFall:~$ ./level8 
(nil), (nil) 
^C
level8@RainFall:~$ ./level8 test test test test
(nil), (nil) 
^C
level8@RainFall:~$ ./level8 test
(nil), (nil) 

(nil), (nil) 

(nil), (nil) 

(nil), (nil) 
^C
level8@RainFall:~$ 
```

# 5. Reverse Engineering (Target Identification):

- Desensamblamos el binario:


Lo que está pasando en LAS pruebas de consola es que:

auth reserva un trozo de memoria.

service reserva otro trozo de memoria después.

Como auth es un puntero, si logramos escribir suficiente "basura" en el heap o reservar suficientes cosas, podemos hacer que la memoria de service caiga exactamente donde el programa espera encontrar los datos de auth.
