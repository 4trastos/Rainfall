# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level5**

```asm
Dump of assembler code for function main:
   0x08048504 <+0>:	    push   ebp
   0x08048505 <+1>:	    mov    ebp,esp
   0x08048507 <+3>:	    and    esp,0xfffffff0
   0x0804850a <+6>:	    call   0x80484c2 <n>
   0x0804850f <+11>:	leave  
   0x08048510 <+12>:	ret    
End of assembler dump.

```

# **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Líneas 0, 1, 3 y 6: **
```asm
0x08048504 <+0>:	push   ebp
0x08048505 <+1>:	mov    ebp,esp
0x08048507 <+3>:	and    esp,0xfffffff0
0x0804850a <+6>:	call   0x80484c2 <n>
```
1. Guarda el valor de EBP (CPU) en lo alto de stack: [esp + 0x00]. Guarda el suelo para no perderlo y el ESP se mueve 4 bytes hacia abajo.
2. Copia el nuevo ESP del stack en el registro EBP como nuevo para la función `main`.
3. Alinea el stack como múltiplo de 16 ybtes. (los ultimos 4 bits de ESP se ponen a 0).
4. Llama la función `n`.

### **Línes 11: `leave`**
```asm
0x0804850f <+11>:	leave
```
- **Equivale a**:
```asm
mov esp,ebp    ; Restaura ESP
pop ebp        ; Restaura EBP antiguo
```
- Deshace el stack frame de la función

### **Línea 12: `ret`** 
```asm
0x08048510 <+12>:	ret 
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`

# **SECCIÓN 2: ANALÁSIS DE LA FUNCIÓN `n`**

```asm
Dump of assembler code for function n:
   0x080484c2 <+0>:	    push   ebp
   0x080484c3 <+1>:	    mov    ebp,esp
   0x080484c5 <+3>:	    sub    esp,0x218
   0x080484cb <+9>:	    mov    eax,ds:0x8049848
   0x080484d0 <+14>:	mov    DWORD PTR [esp+0x8],eax
   0x080484d4 <+18>:	mov    DWORD PTR [esp+0x4],0x200
   0x080484dc <+26>:	lea    eax,[ebp-0x208]
   0x080484e2 <+32>:	mov    DWORD PTR [esp],eax
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484ea <+40>:	lea    eax,[ebp-0x208]
   0x080484f0 <+46>:	mov    DWORD PTR [esp],eax
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>
   0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>
End of assembler dump.
```

### **Líneas: 0, 1 y 3:**
```asm
0x080484c2 <+0>:	    push   ebp
0x080484c3 <+1>:	    mov    ebp,esp
0x080484c5 <+3>:	    sub    esp,0x218
```
1. Guarda el valor de EBP (`main`) en lo alto del stack: [esp + 0x00]. Para no perder ebp anterior.
   (El ESP se mueve 4 bytes hacia abajo).
2. Copia el nuevo ESP del stack para tener un nuevo registro EBP para la función `n`.
3. Reserva 538 bytes (0x218) para el uso de la función `n`.

### **Línea 9: `mov    eax,ds:0x8049848`**
```asm
0x080484cb <+9>:	    mov    eax,ds:0x8049848
```
- Copia en EAX el contenido de la dirección `0x8049848` para la función `fgets()`
- Según el estándar de `fgets(char *s, int size, FILE *stream)`, el tercer argumento es el `stream`.
- `ds:` prefijo de segmento de datos (valor de una variable global).
- Es muy posible que en la dirección `0x8049804` se encuantre `stdin`.

#### **Líneas 14, 18, 26, 32 y 35:**
```asm
0x080484d0 <+14>:	mov    DWORD PTR [esp+0x8],eax
0x080484d4 <+18>:	mov    DWORD PTR [esp+0x4],0x200
0x080484dc <+26>:	lea    eax,[ebp-0x208]
0x080484e2 <+32>:	mov    DWORD PTR [esp],eax
0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>
```
1. ESP mueve 8 bytes hacia abajo y guarda el regisyto EAX (posible `stdin`)
2. ESP mueve 4 bytes más hacia abajo y guarda el limite de lectura del buffer (521 bytes)
3. Calcula donde se encuentra la salida del buffer y lo guarda en EAX. (el buffer completo).
4. Copia EAX en el tope del stack.
5. Llama a la función `fgets()`.

### **Lineas 40, 46 y 49:**
```asm
0x080484ea <+40>:	lea    eax,[ebp-0x208]
0x080484f0 <+46>:	mov    DWORD PTR [esp],eax
0x080484f3 <+49>:	call   0x8048380 <printf@plt>
```
1. Calcula donde se encuentra el buffer leido.
2. Lo copia a lo alto del stack
3. Llama a la función `printf()` y muestra por pantalla lo que hay guardado en el stack `DWORD PTR [esp]`

**Vunerabilidad detectada:** Se está llamando a `printf(buffer)` en lugar de `pritnf("%s", buffer)`. Esto se conoce como : **Format String Vulnerability**. NO necesitamos romper el buffer por tamaño **(buffer overflow)**, sino explotar como `printf` interpreta los datos. **Pero el programa termina con un `exit`. No tenemos ningún `if` ni ninguna función**

### **Líneas 54 y 61:**
```asm
0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1
0x080484ff <+61>:	call   0x80483d0 <exit@plt>
```
1. Copia 1 byte en lo alto del stack.
2. Llama al syscall `exit`.

**Exploit detectado:** Podemos sobreescribir la dirección de una función en la *GOT* (Global Offset Table).
- **La .got:** Es una tabla donde el programa guarda las direcciones reales de las funciones de las librerías dinámicas: `printf`, `fgets`, `exit`, etc.
- Si cambiamos la direcciñón de `exit` en el registro `.got` por la dirección de alguna función que nos de la shell, cuando el programa ejecute `call exit@plt` en realidad saltará a donde queramos.

```bash
level5@RainFall:~$ readelf -S ./level5 | grep -E ".got|.plt"
  [10] .rel.plt          REL             080482fc 0002fc 000038 08   A  5  12  4
  [12] .plt              PROGBITS        08048370 000370 000080 04  AX  0   0 16
  [22] .got              PROGBITS        08049814 000814 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049818 000818 000028 04  WA  0   0  4

```
| `WA` -> Confirma sección `.got` en escritura |

### **Resumen del Flujo de Ataque para el Nivel 5:**

1. **Lectura:** `main` solo alinea el stack y llama a la función `n`. La función `n` lee con `fgets()` y realiza la impresión.
2. **Entrada:** `fgets` limita la lectura impidiendo el tradicional *Buffer Overflow*.
3. **Vulnerabilidad:** Usaremos `printf()` recibe nuestro buffer sin un string de formato fijo.
4. **Explotación:** Sobreescribir en el *.got* la dirección de `exit` para cuando lo ejecute nos salte al binario que ejecute: `system("/bin/sh")`