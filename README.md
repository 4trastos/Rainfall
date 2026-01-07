# Rainfall: Binary Exploitation Wargame

## 📝 Descripción del Proyecto

**Rainfall** es el segundo gran reto de seguridad en la línea de formación de 42. Mientras que Snow Crash introdujo conceptos generales de seguridad, Rainfall profundiza exclusivamente en la **explotación de binarios** (Binary Exploitation / Pwn).

El objetivo es diseccionar ejecutables escritos en C para encontrar vulnerabilidades críticas en la gestión de memoria y flujo de ejecución, permitiendo escalar privilegios desde el usuario `level0` hasta `bonus3`.

---

## 🛠️ Objetivos de Aprendizaje

* **Ingeniería Inversa:** Uso avanzado de `gdb` (GNU Debugger) y herramientas de desensamblado para entender código sin acceso al fuente.
* **Manipulación de la Memoria:** Comprender la estructura del *Stack* (pila) y el *Heap* (montículo).
* **Vulnerabilidades de Corrupción de Memoria:**
* **Buffer Overflow:** Sobrescritura de buffers para controlar el registro `EIP`.
* **Format String Attacks:** Explotación de funciones tipo `printf` para leer o escribir en cualquier dirección de memoria.
* **Return-to-libc (ret2libc):** Ejecución de funciones de la biblioteca estándar saltándose protecciones.


* **Análisis de Registros:** Dominio de registros de la arquitectura x86 (EAX, ESP, EBP, EIP).

---

## 🚀 Metodología de Trabajo

Para cada nivel, se sigue una metodología rigurosa:

1. **Reconocimiento:** Identificación de permisos, funciones importadas y comportamiento del binario.
2. **Análisis Estático:** Lectura del código desensamblado (ASM) para localizar puntos débiles (uso de `gets`, `strcpy`, `printf` inseguros).
3. **Análisis Dinámico:** Depuración con `gdb` para observar el estado de la memoria en tiempo de ejecución.
4. **Desarrollo del Exploit:** Creación de *payloads* (cadenas de bytes) diseñados específicamente para desviar el flujo del programa hacia nuestro objetivo.

---

## 🧰 Herramientas Utilizadas

* **GDB:** Indispensable para inspeccionar registros y memoria.
* **Objdump / Nm:** Para el análisis de símbolos y secciones del ejecutable.
* **Ltrace / Strace:** Para interceptar llamadas a librerías y al sistema.
* **Python:** Para la generación de payloads complejos y gestión de bytes no imprimibles.

---

## 📂 Estructura del Repositorio

Cada nivel está organizado en su propia carpeta conteniendo:

* **source.c:** Reconstrucción del código fuente original a partir del desensamblado.
* **exploit.py / payload.txt:** El método utilizado para romper el binario.
* **README.md:** Explicación técnica detallada del paso a paso y la lógica aplicada.

---

> "En Rainfall, la memoria no es solo un lugar donde se guardan datos, es el tablero de juego donde cada byte cuenta."

---

Para empezar con buen pie, te recomiendo que en tu entorno de Rainfall (que suele ser una ISO de 32 bits) te familiarices con un par de comandos de `gdb` que vas a usar constantemente:

1. `info registers`: Para ver qué hay en los registros.
2. `x/20wx $esp`: Para ver qué hay en el tope de la pila.
3. `disas main`: Para ver el código ensamblador de la función principal.
