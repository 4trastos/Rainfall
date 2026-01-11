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

# 5. Identity Target:

```
level0@RainFall:~$ gdb ./level0 
(gdb) set disassembly-flavor intel
(gdb) disas main
```

# 6. Analizamos el binario línea por línea:

Para ver con detalle el analisis consuta el archivo  [asm_analysis.md](https://github.com/4trastos/Rainfall/blob/main/level0/Resources/README.md) en conjunto con el programa de demostración  [source.c](https://github.com/4trastos/Rainfall/blob/main/level0/source.c).  





Está quedando **muy profesional**. La estructura es limpia, lógica y sigue la metodología que un auditor de seguridad real utilizaría.

Sin embargo, para que sea "perfecto" y no te pillen en ninguna pregunta durante la evaluación, debemos corregir **dos detalles técnicos** en tu tabla de resumen y en la sección de NX.

Aquí tienes las correcciones sugeridas:

---

### 1. Corrección en la tabla (Sección NX)

En tu tabla de resumen pusiste:

> **NX | ENABLED | GNU_STACK RW | Podemos ejecutar código en el Stack**

**¡Cuidado!** Eso es una contradicción. Si el NX está **ENABLED** (activado), significa que **NO** podemos ejecutar código en el Stack.

* **Corrección:**
| Protección | Estado | Resultado | Consecuencias |
| --- | --- | --- | --- |
| **NX** | **ENABLED** | `RW` | **NO** se puede ejecutar código en el Stack |

---

### 2. Corrección en el punto 2.c (NX)

Has puesto: `readelf -l ./level0 | grep -E "STACK|RELRO"`.
Como vimos antes, el hecho de que solo salga `STACK` y no `RELRO` ya confirma que el RELRO es inexistente.

---

### 3. Mejora en el punto 4 (Fuzzing / Análisis de errores)

Es importante explicar **por qué** da `Segmentation fault` sin argumentos.

* **Añade esta nota:** "El `segfault` ocurre porque el programa intenta pasar un puntero nulo (`NULL`) a la función `atoi` cuando no enviamos argumentos, provocando un error de lectura en la memoria."

---

### Así quedaría tu Walkthrough mejorado (puedes copiar y pegar):

# 1. Buscar Binario

Listamos los archivos para identificar el binario `level0` y sus permisos.

```bash
level0@RainFall:~$ ls -l
total 732
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0

```

> **Nota:** El bit `s` en los permisos indica que es un binario **SUID**. Se ejecutará con los privilegios del propietario (`level1`).

---

# 2. Analizamos las protecciones de la RAM

### a) PIE (Position Independent Executable)

```bash
level0@RainFall:~$ file ./level0
# Resultado: executable (No PIE) -> Direcciones fijas.

```

### b) ASLR (Address Space Layout Randomization)

```bash
level0@RainFall:~$ cat /proc/sys/kernel/randomize_va_space 
# Resultado: 0 -> Desactivado a nivel de sistema.

```

### c) NX (No-Execute) y RELRO

```bash
level0@RainFall:~$ readelf -l ./level0 | grep -E "STACK|RELRO"
GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x4
# Resultado: RW (sin E) -> NX activado. No aparece RELRO.

```

### d) STACK CANARY

```bash
level0@RainFall:~$ nm ./level0 | grep stack_chk
# Resultado: Error / No results -> Comprobación manual requerida en GDB.

```

### e) .got y .plt

```bash
level0@RainFall:~$ readelf -S ./level0 | grep -E ".got|.plt"
# Resultado: WA en .got.plt -> La tabla de funciones es escribible.

```

---

# 3. Resumen del análisis

| Protección | Estado | Evidencia | Consecuencia |
| --- | --- | --- | --- |
| **PIE** | **OFF** | `executable` | El binario siempre carga en `0x08048000`. |
| **ASLR** | **OFF** | `0` | Las direcciones de librerías y stack son constantes. |
| **NX** | **ON** | `RW` | **No** podemos ejecutar shellcode en el stack. |
| **RELRO** | **OFF** | `No RELRO` | La tabla GOT puede ser sobrescrita. |
| **CANARY** | **OFF** | `disas main` | No hay protecciones contra desbordamiento de pila. |

---

# 4. Fuzzing inicial

Probamos el comportamiento del binario con diferentes entradas:

* `./level0`: **Segmentation fault**. (Fallo al intentar procesar un argumento inexistente en `atoi`).
* `./level0 test`: **No !**. (El argumento no coincide con la clave esperada).

---

# 5. Reverse Engineering (Target Identification)

Entramos en GDB para entender la lógica interna y encontrar la vulnerabilidad o la "puerta trasera".

```bash
level0@RainFall:~$ gdb -q ./level0 
(gdb) set disassembly-flavor intel
(gdb) disas main

```

---

### ¿Qué te parece esta versión?

Está un poco más pulida técnicamente. Si estás de acuerdo, el siguiente paso natural en tu walkthrough sería el **"Capítulo 6: La Solución"**, donde explicas que encontraste el valor `0x1a7` (423) y cómo lo ejecutaste para ganar.

¿Quieres que redactemos ese paso final o prefieres ir preparando el entorno para el **Level 1**?