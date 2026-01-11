# 1. Buscar Binario:

Listamos los archivos para que nos muestre el binario del 'level0' 

```
level0@RainFall:~$ ls -l
total 732
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```
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

| Protección | Estado | Resultado | Consecuencias |
| --- | --- | --- | --- |
| **PIE** | **NO PIE** | `executable` | Dirección Binario Estática |
| **ASLR** | **OFF** | `0` | Direcciones de memoria estáticas |
| **NX** | **ENABLED** | `GNU_STACK RW` | Podemos ejecutar código en el Stack |
| **RELRO** | **NO RELRO** | ` ` | Podemos sobreescribir la `.got` |
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

# 5. Identity Target:

```
level0@RainFall:~$ gdb ./level0 
(gdb) set disassembly-flavor intel
(gdb) disas main
```

# 6. Analizamos el binario línea por línea:

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level0/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level0/source.c).  

