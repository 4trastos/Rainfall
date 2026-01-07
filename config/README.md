
# 🛡️ Guía Profesional de Ataque - Rainfall

Esta guía detalla las protecciones de seguridad modernas en sistemas Linux x86 y cómo su estado (Enabled/Disabled) determina la estrategia de explotación.

---

## 1. Análisis de Protecciones de Memoria

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
