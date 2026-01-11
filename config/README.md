
# 🛡️ Guía Profesional de Ataque - Rainfall

Esta guía detalla las protecciones de seguridad modernas en sistemas Linux x86 y cómo su estado (Enabled/Disabled) determina la estrategia de explotación.

---

## 1. Análisis de Protecciones de Memoria


```bash
checksec --file level0
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   level0

```

Cuando inicias sesión en un nivel de Rainfall, el banner te muestra el estado de seguridad. Aquí tienes el desglose técnico de cada una:

### **ASLR (Address Space Layout Randomization)**

* **Qué es:** Aleatoriza las direcciones base de la pila (stack), el montón (heap) y las librerías compartidas (`libc`) cada vez que se ejecuta el programa.
* **En Rainfall (OFF):** Es nuestra mayor ventaja. Las direcciones de memoria son estáticas. Si descubres que `system()` está en `0xb7e6b060`, estará ahí en cada ejecución.
* **Si estuviera ON:** Necesitaríamos un "Information Leak" (una vulnerabilidad que nos revele una dirección de memoria en tiempo de ejecución) para calcular los desplazamientos (offsets).

### **NX (No-Execute / DEP)**

* **Qué es:** Marca ciertas regiones de memoria (como el Stack) como "no ejecutables". Evita que un atacante inyecte código (shellcode) en un buffer y salte a él para ejecutarlo.
* **En Rainfall (ENABLED):** No podemos ejecutar código en la pila.
* **Estrategia:** Usaremos **Ret2Libc** (saltar a funciones ya existentes en la librería estándar) o **ROP** (Return Oriented Programming), usando "gadgets" de código que ya están marcados como ejecutables.

### **Stack Canaries (Stack Guard)**

* **Qué es:** Un valor aleatorio (el "canario") colocado en la pila justo antes de la dirección de retorno (`EIP`). Antes de que una función termine, el programa comprueba si el canario sigue ahí. Si ha cambiado (por un desbordamiento), el programa aborta (`__stack_chk_fail`).
* **En Rainfall (NOT FOUND):** Podemos sobrescribir el `EIP` directamente mediante un Buffer Overflow sin preocuparnos por "matar al canario".
* **Si estuviera ENABLED:** Necesitaríamos filtrar el valor del canario o usar técnicas para saltárnoslo.

### **PIE (Position Independent Executable)**

* **Qué es:** Similar al ASLR, pero para el código del binario mismo. Hace que la dirección base del ejecutable cambie en cada ejecución.
* **En Rainfall (NO PIE):** El binario siempre se carga en la dirección `0x08048000`. Esto hace que las direcciones de las funciones locales sean fijas y fáciles de atacar.

### **RELRO (Relocation Read-Only)**

* **Qué es:** Protege la sección `GOT` (Global Offset Table), que es donde el binario guarda las direcciones de las funciones de librerías externas.
* **Partial RELRO:** La sección `GOT` está antes que las variables globales, dificultando que un desbordamiento de variable la sobrescriba, pero sigue siendo vulnerable a ataques de *Format String*.
* **Full RELRO:** La `GOT` se vuelve de solo lectura tras cargarse.


* **En Rainfall (NO RELRO):** Podemos sobrescribir direcciones en la `GOT` para que, cuando el programa llame a una función legítima (como `printf`), en realidad ejecute nuestra función maliciosa (como `system`).

---

## 2. Los Registros: El Mapa de la CPU (x86 32-bit)

Para moverte por el código desensamblado, estos son tus puntos de referencia:

* **EAX**: Registro acumulador (donde se suelen guardar los **valores de retorno** de las funciones).
* **ESP (Stack Pointer)**: Apunta a la **cima** de la pila actual. Crucial para ver dónde caen nuestros datos inyectados.
* **EBP (Base Pointer)**: Apunta a la base del *stack frame* de la función actual. Marca el inicio del marco de la función actual.
* **EIP (Instruction Pointer)**: **El más importante.** Es el "puntero de ejecución". Contiene la dirección de la siguiente instrucción a ejecutar. Quien controla el EIP, controla el programa. **Nuestro objetivo es siempre sobrescribirlo.** 

#### La Pila (Stack) y las llamadas:

1. **Push**: Mete algo en la pila (ESP baja).
2. **Pop**: Saca algo de la pila (ESP sube).
3. **Call**: Mete el EIP actual en la pila (dirección de retorno) y salta a la función.
4. **Leave**: Limpia el stack frame actual (`mov esp, ebp` y `pop ebp`).
5. **Ret**: Saca la dirección de retorno de la pila y la mete en el EIP.

### El Mapa del Hardware

* **Registros (CPU):** Son las "celdas de memoria" más rápidas que existen. Están dentro del procesador. El procesador no puede operar directamente sobre la RAM; tiene que traer los datos a los registros, operar con ellos y luego devolverlos (si es necesario).
* **Stack y Heap (RAM):** Son regiones de la memoria principal. La RAM es mucho más grande que los registros, pero más lenta. El sistema operativo divide la RAM asignada a un programa en diferentes "segmentos".

---

### ¿Qué es el Heap? (El Montículo)

Si el **Stack** es una pila de platos organizada (LIFO: el último en entrar es el primero en salir), el **Heap** es un "gran almacén" de espacio libre.

* **Propósito:** Se usa para la **asignación dinámica de memoria**. Es donde van los datos cuyo tamaño no conocemos antes de ejecutar el programa o que son demasiado grandes para el Stack.
* **Gestión:** En C, tú controlas el Heap manualmente usando `malloc()`, `calloc()` y liberándolo con `free()`.
* **Dirección de crecimiento:** Mientras que el **Stack crece hacia abajo** (hacia direcciones de memoria menores), el **Heap crece hacia arriba** (hacia direcciones mayores). Están diseñados para "encontrarse" en el medio del espacio de memoria si se llenan.

---

### Comparativa: Stack vs. Heap


| Característica | Stack (Pila) | Heap (Montículo) |
| --- | --- | --- |
| **Tipo de asignación** | Automática (por el compilador). | Manual (por el programador). |
| **Velocidad** | Muy rápida. | Más lenta (requiere gestión del SO). |
| **Orden** | Estricto (LIFO). | Desordenado (fragmentación). |
| **Uso común** | Variables locales y direcciones de retorno. | Estructuras grandes, listas enlazadas, objetos. |
| **Vulnerabilidad** | **Buffer Overflow** (sobrescribir el EIP). | **Heap Overflow / Use After Free** (más complejo). |

---

### ¿Por qué nos importa el Heap en Rainfall?

Aunque en los primeros niveles de Rainfall nos centraremos casi siempre en el **Stack** (porque ahí es donde reside el registro **EIP** que queremos controlar), en niveles avanzados podríamos encontrar:

1. **Heap Overflow:** Si sobrescribes datos en el Heap, puedes corromper punteros a funciones o metadatos del gestor de memoria.
2. **Uso de memoria dinámica:** Si un programa usa `malloc`, los datos estarán allí. A veces, la "llave" para pasar de nivel está guardada en el Heap y no en el Stack.

> "El **Stack** es donde vive el **control** (direcciones de retorno), el **Heap** es donde viven los **datos masivos** (contenido dinámico) y los **Registros** son el **motor** que mueve todo."

---

## 3. Metodología de Explotación Recomendada

1. **Checksec:** Analizar qué protecciones están OFF (lo primero que ves al entrar).
2. **Fuzzing:** Introducir cadenas largas de caracteres (ej. `python -c "print 'A'*100"`) para ver si el programa rompe (`Segmentation Fault`).
3. **Find Offset:** Determinar exactamente cuántos caracteres se necesitan para llegar al `EIP`.
4. **Identify Target:** ¿A dónde queremos saltar?
* ¿Hay una función `secret()` o `shell()` en el código? (Salto directo).
* ¿Tenemos que saltar a `system("/bin/sh")` en la `libc`? (Ret2Libc).


5. **Craft Payload:** Construir la cadena de ataque y ejecutar.


---

### Siguiente paso para el estudio:

Para estrenar esta guía, ¿por qué no lanzas un `gdb ./level0` y haces un `disas main`? Verás el código en ensamblador, te ayudaré a aplicar el punto **3 (Metodología)** para ver cómo romper este primer binario.

### BONUS. Primeros pasos con `level0`

Vamos a examinar ese binario. Ejecuta estos comandos para empezar a ver qué hace:

1. **Análisis estático inicial**:
```bash
ls -la        # Mira permisos (¿Es SUID?)
file level0   # Confirma que es un ELF de 32 bits

```


2. **Entrar en GDB**:
```bash
gdb -q ./level0

```


3. **Dentro de GDB**, vamos a ver el código:
```gdb
set disassembly-flavor intel    # Para leerlo más cómodo
disas main

```

Fíjate especialmente si ves funciones como `system`, `execv`, `atoi` o comparaciones (`cmp`).

# AYUDA:

### 1. La Herramienta Maestra: `checksec`

`checksec` es un script (normalmente parte del paquete `pwntools` o instalado de forma independiente) que analiza el archivo ELF y te devuelve exactamente el cuadro que viste, pero con colores.

Si estás en un sistema moderno (como Kali Linux o Ubuntu), puedes usarlo así:

```bash
checksec --file=/path/to/binary

```

**¿Qué hace por detrás?**
Revisa las "cabeceras" del binario. Por ejemplo:

* Busca la sección `.stack_prot` para saber si hay **Canaries**.
* Mira el bit `GNU_STACK` en los segmentos para ver si el **NX** está activo.
* Comprueba si el tipo de archivo es `EXEC` (No PIE) o `DYN` (PIE enabled).

---

### 2. Cómo verlo manualmente (Si no tienes `checksec`)

Si estás en una máquina restringida (como las de 42 o un servidor antiguo) y no puedes instalar nada, puedes usar las herramientas integradas del sistema para deducir las protecciones:

#### A. Para NX (No-Execute)

Usa `readelf` para buscar la pila:

```bash
readelf -l ./level0 | grep -A 1 STACK

```

* Si ves `RW`, el stack es de lectura/escritura (NX **Enabled**, No puedes ejecutar código inyectado en el stack. Debes usar técnicas como Ret2Libc.).
* Si ves `RWE`, el stack es ejecutable (NX **Disabled**) Sí puedes ejecutar código inyectado en el stack (shellcode). Es el escenario más sencillo.

#### B. Para PIE (Position Independent Executable)

```bash
file ./level0

```

* Si dice `executable`, es **No PIE** (dirección fija).
* Si dice `shared object`, es **PIE** (dirección aleatoria).

#### C. Para ASLR (A nivel de Sistema Operativo)

El ASLR no depende del binario, sino del Kernel. Puedes ver su estado así:

```bash
cat /proc/sys/kernel/randomize_va_space

```

* `0`: **Disabled** (como en Rainfall).
* `1`: Conservador (aleatoriza stack y librerías).
* `2`: Full (incluye el heap).

#### D. Para RELRO (Relocation Read-Only)

Usa `readlelf` para un segmento específico llamado GNU_RELRO en los "Program Headers":

```bash
readelf -l ./level0 | grep RELRO || echo "NO hay RELRO"

```

* NO aparece `GNU_RELRO`: **NO RELRO** La tabla `.got.plt` es totalmente vulnerable y se puede sobrescribir en cualquier momento.
* `GNU_RELRO`con `Flg R` (ReadOnly):Sería **Partial RELRO** o **Full RELRO**.

#### E. Análisis de Secciones Críticas (`.plt`, `.got`)

Usa `readelf` para obterne información de las secciones importantes

```bash
readelf -S ./level0 | grep -E "stack|.got|.plt"

```
Obtenemos el mapa de cómo el programa gestiona las funciones externas (como `atoi`, `execv`, etc.).

```bash
[ 3] .rel.plt          REL             08048118 000118 000088 08   A  0   5  4
[ 5] .plt              PROGBITS        080481d0 0001d0 000110 00  AX  0   0 16
[24] .got              PROGBITS        080ee0f0 0a50f0 000008 04  WA  0   0  4
[25] .got.plt          PROGBITS        080ee0f8 0a50f8 000050 04  WA  0   0  4

```

1. **`.plt` (Procedure Linkage Table):**
* **Flag `AX` (Alloc/Execute):** Es una sección **ejecutable**.
* **Función:** Es un "trampolín". Cuando el código llama a `atoi`, no salta directamente a la librería de C. Salta a una entrada en la `.plt`, que luego consulta la dirección real en la `.got`.


2. **`.got` (Global Offset Table):**
* **Flag `WA` (Write/Alloc):** Es una sección de **Escritura**.
* **Función:** Es una tabla de direcciones. Aquí es donde se guarda la dirección real de memoria de las funciones externas.


3. **`.got.plt`:**
* **Flag `WA` (Write/Alloc):** También es de **Escritura**.
* **Peligro:** Si esta sección tiene permiso de escritura (`W`), significa que un atacante puede sobrescribir una dirección (por ejemplo, cambiar la dirección de `atoi` por la de `system`) para tomar el control.

#### G. Para detectar el Canary manualmente STACK CANARIES

1. Búsqueda de símbolos (Rápida): `nm ./level0 | grep stack_chk`. busca la función de error que se dispara cuando el canario "muere".

* **Resultado:** Si aparece `__stack_chk_fail`, el binario tiene protección contra desbordamientos.

2. Si `nm ` no funciona, usa `disas main` dentro de `gdb` para buscar el canario dentro del desensamblado:

```bash
gdb ./level0

(gdb) set disassembly-flavor intel
(gdb) disas main
```
* Lo que debemos buscar:

```
# AL INICIO (Prólogo de la función):
0x08048f00:  mov    eax, gs:0x14      ; Lee el valor secreto (Canario) desde el Sistema Operativo
0x08048f06:  mov    [ebp-0x8], eax    ; Guarda una copia del Canario en el Stack

# ... código de la función ...

# AL FINAL (Epílogo de la función):
0x08048f40:  mov    eax, [ebp-0x8]    ; Recupera la copia del Canario que guardamos
0x08048f43:  xor    eax, gs:0x14      ; Compara el valor del Stack con el original de GS
0x08048f49:  je     0x08048f52        ; Si son iguales (XOR = 0), todo está OK: salta al final
0x08048f4b:  call   __stack_chk_fail  ; Si NO son iguales, el programa aborta (Ataque detectado)

```

* Si en `disas main` no viste ninguna llamada a `__stack_chk_fail` ni operaciones con `gs:0x14`, entonces **NO hay canario**.

---

> ### 🛠️ Comandos de Reconocimiento Rápido
> 
> 
> 1. **`checksec --file=[binario]`**: El método rápido y visual.
> 2. **`readelf -Wl [binario]`**: Para ver los segmentos del programa y permisos de memoria.
> 3. **`nm -D [binario]`**: Para ver las funciones dinámicas (si ves `system` o `gets`, ya tienes una pista del ataque).
> 4. **`ldd [binario]`**: Para ver en qué direcciones se cargan las librerías (si las direcciones cambian al repetir el comando, el ASLR está ON).
> 
> 

---
