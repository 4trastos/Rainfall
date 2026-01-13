# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level0' y sus permisos. 

```
level0@RainFall:~$ ls -l
total 732
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level1`).

---

# 2. Analizamos las protecciones de la RAM

### a). PIE;
```
flevel0@RainFall:~$ file ./level0
./level0: setuid ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.24, BuildID[sha1]=0x85cf4024dbe79c7ccf4f30e7c601a356ce04f412, not stripped

```

### b). ASLR:
```
level0@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
0
```

### c). NX y RELRO:
```
flevel0@RainFall:~$ readelf -l ./level0 | grep -E "STACK|RELRO"
GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x4
```

### d). STACK CANARY:
```
level0@RainFall:~$ nm ./level0 | grep stack_chk
-bash: /usr/bin/nm: Input/output error
```

### e). `.got` y `.plt`:
```
level0@RainFall:~$ readelf -S ./level0 | grep -E ".got|.plt"
  [ 3] .rel.plt          REL             08048118 000118 000088 08   A  0   5  4
  [ 5] .plt              PROGBITS        080481d0 0001d0 000110 00  AX  0   0 16
  [24] .got              PROGBITS        080ee0f0 0a50f0 000008 04  WA  0   0  4
  [25] .got.plt          PROGBITS        080ee0f8 0a50f8 000050 04  WA  0   0  4
```
---

# 3. Resumen del análisis:

| Protección | Estado | Evidencia | Consecuencias |
| --- | --- | --- | --- |
| **PIE** | **OFF** | `executable` | El binario siempre carga en `0x08048000` |
| **ASLR** | **OFF** | `0` | Las direcciones de librerías y stack son constantes. |
| **NX** | **ON** | `GNU_STACK RW` | No podemos ejecutar código (ShellCode) en el Stack |
| **RELRO** | **OFF** | `No RELRO ` | La tabla GOT puede ser sobrescrita. |
| **CANARY** | **? ? ?**  | `error` | Buscar con `gdb` |
| **.got.plt** | **WA**  | `WA` | Confirma sección `.got` en escritura |

---

# 4. Fuzzing:

Introducimos cadenas de caracteres para probar si el programa rompe:

```
level0@RainFall:~$ ./level0 
Segmentation fault (core dumped)

level0@RainFall:~$ ./level0 test
No !

level0@RainFall:~$ ./level0 test ettes
No !

```

* El binairo `level0` genera un error de `segfault` sin arguemntos e imprime "No !" con uno o varios argumentos.

---

# 5. Reverse Engineering (Target Identification):

Entramos en GDB para entender la lógica interna y encontrar la vulnerabilidad o la "puerta trasera".

```
level0@RainFall:~$ gdb ./level0 
(gdb) set disassembly-flavor intel
(gdb) disas main
```

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level0/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level0/source.c).
  
---

# 6. Solución:

Tras analizar el desensamblado en el punto anterior, identificamos una comparación lógica crítica:

```asm
0x08048ed9 <+25>:    cmp    eax,0x1a7
0x08048ede <+30>:    jne    0x8048f58 <main+152>

```

### Análisis del hallazgo:

1. El programa toma nuestro primer argumento (`argv[1]`) y lo convierte a entero mediante `atoi`.
2. El resultado se almacena en el registro **EAX**.
3. Se compara **EAX** con el valor hexadecimal **`0x1a7`**.
4. Convertimos el valor a decimal: `$0x1a7$ = **423**`.
5. Si el valor coincide, el programa no salta (`jne`) y procede a ejecutar `setresuid` (escalada de privilegios) y `execv` (lanzamiento de shell).

### Ejecución del exploit:

Pasamos el número "mágico" como argumento al binario:

```bash
level0@RainFall:~$ ./level0 423
$ whoami
flag00
$ getflag
Check flag.Here is your token : [AQUÍ_IRÁ_TU_TOKEN]

```

---

# 7. Conclusión:

El nivel 0 no requería una explotación de memoria compleja (como Buffer Overflow), sino un ejercicio de **Ingeniería Inversa** para entender la lógica de control del programa. Al identificar la constante esperada, pudimos forzar la ejecución de la "puerta trasera" programada en el binario.

---