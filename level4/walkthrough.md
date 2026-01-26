# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level4' y sus permisos. 

```bash
level4@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level4 level4   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level4 level4  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level4 level4 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level5 users  5252 Mar  6  2016 level4
-rw-r--r--+ 1 level4 level4   65 Sep 23  2015 .pass
-rw-r--r--  1 level4 level4  675 Apr  3  2012 .profile
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level5`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level4@RainFall:~$ file ./level4 
./level4: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xf8cb2bdaa7daab1347b36aaf1c98d49529c605db, not stripped
```

### b). ASLR:
```bash
level3@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level4@RainFall:~$ readelf -l ./level4 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level3@RainFall:~$ nm ./level4 
bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level4@RainFall:~$ readelf -S ./level4 | grep -E ".got|.plt"
  [10] .rel.plt          REL             080482d0 0002d0 000028 08   A  5  12  4
  [12] .plt              PROGBITS        08048330 000330 000060 04  AX  0   0 16
  [22] .got              PROGBITS        080497d8 0007d8 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        080497dc 0007dc 000020 04  WA  0   0  4

```

---

# 3. Resumen del análisis:

| Protección | Estado | Evidencia | Consecuencias |
| --- | --- | --- | --- |
| **PIE** | **OFF** | `executable` | El binario siempre carga en `0x08048000` |
| **ASLR** | **OFF** | `0` | Las direcciones de librerías y stack son constantes. |
| **NX** | **OFF** | `GNU_STACK RWE` | Si podemos ejecutar código (ShellCode) en el Stack |
| **RELRO** | **OFF** | `No RELRO ` | La tabla GOT puede ser sobrescrita. |
| **CANARY** | **? ? ?**  | `error` | Buscar con `gdb` |
| **.got.plt** | **WA**  | `WA` | Confirma sección `.got` en escritura |

---

# 4. Fuzzing:


# 5. Reverse Engineering (Target Identification):


Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level4/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level4/source.c).

# 6. Solución: Format String Attack.

---

# **7. Conclusión:**