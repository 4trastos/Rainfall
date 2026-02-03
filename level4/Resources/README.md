# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level4**

```asm
Dump of assembler code for function main:
   0x080484a7 <+0>:	push   ebp
   0x080484a8 <+1>:	mov    ebp,esp
   0x080484aa <+3>:	and    esp,0xfffffff0
   0x080484ad <+6>:	call   0x8048457 <n>
   0x080484b2 <+11>:	leave  
   0x080484b3 <+12>:	ret    
End of assembler dump.
```

# **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **línea 0: `push   ebp`**
```asm
0x080484a7 <+0>:	push   ebp
```
- Guarda el valor de EBP (CPU) en lo alto del stack: [ebp + 0x00]. Guarda el suelo anterior para no perderlo.
- El ESP se mueve 4 bytes hacia abajo

### **línea 1: `mov    ebp,esp`**
```asm
0x080484a8 <+1>:	mov    ebp,esp
```
- Copia el nuevo ESP del stack para tener un nuevo registros para la función `main`

### **Línea 3: `and    esp,0xfffffff0`**
```asm
0x080484aa <+3>:	and    esp,0xfffffff0
```
- Alinea el stack a múltiplo de 16 bytes.
- Los últimos 4 bits de ESP se ponene a 0 (ej: 0xbffff7c9 → 0xbffff7c0)

### **Línea 6: `0x080484ad <+6>:	call   0x8048457 <n>`**
```asm
0x080484ad <+6>:	call   0x8048457 <n>
```
- Llama la función `n`

### **Línea 11: `leave`**
```asm
0x080484b2 <+11>:	leave   
```
- **Equivalente a**:
```asm
mov esp,ebp    ; Restaura ESP
pop ebp        ; Restaura EBP antiguo
```
- Deshace el stack frame de la función

### **Línea 12: `ret` **
```asm
0x080484b3 <+12>:	ret 
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`

# **SECCIÓN 2: ANALÁSIS DE LA FUNCIÓN `n`**

```asm
Dump of assembler code for function n:
   0x08048457 <+0>:	push   ebp
   0x08048458 <+1>:	mov    ebp,esp
   0x0804845a <+3>:	sub    esp,0x218
   0x08048460 <+9>:	mov    eax,ds:0x8049804
   0x08048465 <+14>:	mov    DWORD PTR [esp+0x8],eax
   0x08048469 <+18>:	mov    DWORD PTR [esp+0x4],0x200
   0x08048471 <+26>:	lea    eax,[ebp-0x208]
   0x08048477 <+32>:	mov    DWORD PTR [esp],eax
   0x0804847a <+35>:	call   0x8048350 <fgets@plt>
   0x0804847f <+40>:	lea    eax,[ebp-0x208]
   0x08048485 <+46>:	mov    DWORD PTR [esp],eax
   0x08048488 <+49>:	call   0x8048444 <p>
   0x0804848d <+54>:	mov    eax,ds:0x8049810
   0x08048492 <+59>:	cmp    eax,0x1025544
   0x08048497 <+64>:	jne    0x80484a5 <n+78>
   0x08048499 <+66>:	mov    DWORD PTR [esp],0x8048590
   0x080484a0 <+73>:	call   0x8048360 <system@plt>
   0x080484a5 <+78>:	leave  
   0x080484a6 <+79>:	ret    
End of assembler dump.

```

### **Lína 0: `push   ebp`**
```asm
0x08048457 <+0>:	push   ebp
```
- Guarda el valor de EBP (`main`) en lo alto del stack: [ebp + 0x00]. Guarda el suelo para no perderlo.
- El ESP se mueve hacia abajo 4 bytes (arquitectura i386 (x86) de 32bits)

### **Línea 1: `mov    ebp,esp`**
```asm
0x08048458 <+1>:	mov    ebp,esp
```
- Copia el nuevo ESP del stack para tener un nuevo registro para la función `n`

### **Línea 3: `sub    esp,0x218`**
```asm
0x0804845a <+3>:	sub    esp,0x218
```
- Reserva 538 bytes (0x218) para el uso de la función `n`

### **Línea 9: `mov    eax,ds:0x8049804`**
```asm
0x08048460 <+9>:	mov    eax,ds:0x8049804
```
- Copia en EAX el contenido de la dirección `0x8049804` (para la función `fgets`)
- Según el estándar de `fgets(char *s, int size, FILE *stream)`, el tercer argumento es el stream
- `ds`: prefijo de segmento de datos (valor de una variable global).
- Es posible que en la direccion `0x8049804` se encuentre `stdin` ya que el programa espera que escribamos algo.

### **Líneas 14, 18, 26, 32 y 35: **
```asm
0x08048465 <+14>:	mov    DWORD PTR [esp+0x8],eax
0x08048469 <+18>:	mov    DWORD PTR [esp+0x4],0x200
0x08048471 <+26>:	lea    eax,[ebp-0x208]
0x08048477 <+32>:	mov    DWORD PTR [esp],eax
0x0804847a <+35>:	call   0x8048350 <fgets@plt>
```
- 1. ESP mueve 8 bytes hacia abajo y guarda el registro EAX
- 2. ESP mueve 4 bytes más hacia abajo y guarda el límite de lectura del buffer (512 byets)
- 3. Calcula donde se encuentra la salida del buffer y lo guarda en EAX. (el buffer completo)
- 4. Guarda EAX en el tope del stack.
- 5. Llama a la función `fgets`.

### **Líneas 40, 46 y 49:**
```asm
0x0804847f <+40>:	lea    eax,[ebp-0x208]
0x08048485 <+46>:	mov    DWORD PTR [esp],eax
0x08048488 <+49>:	call   0x8048444 <p>
```
- 1. Calcula donde se encuentra el buffer leido
- 2. Guarda EAX al tope del stack
- 3. Llama a la función `p`

### **Líneas 54, 59, 64, 66 y 73:**
```asm
0x0804848d <+54>: mov    eax,ds:0x8049810
0x08048492 <+59>: cmp    eax,0x1025544
0x08048497 <+64>: jne    0x80484a5 <n+78>
0x08048499 <+66>: mov    DWORD PTR [esp],0x8048590
0x080484a0 <+73>: call   0x8048360 <system@plt>
```
- 1. Carga el valor en EAX de una varible global (está en el segmento de datos `.data` o `.bss`)
**La Variable Global m:** Se define fuera de cualquier función. Esto la sitúa en el segmento de datos (.data o .bss), lo que garantiza que su dirección de memoria (0x08049810) sea fija y no se vea afectada por el ASLR del stack.
- 2. Compara ese valor con la constante 16930116 (valor en decimal)
- 3. Si NO son iguales, salta al final y el programa termina.
- 4. Copia la dirección `0x8048590` al inicio del stack
- 5. Ejecuta `system("/bin/sh")`, la dirección `0x8048590` apunta al string `/bin/sh` o `cat /home/user/level5/.pass`

# **SECCIÓN 3: ANALÁSIS DE LA FUNCIÓN `p`**
```asm
Dump of assembler code for function p:
   0x08048444 <+0>:	push   ebp
   0x08048445 <+1>:	mov    ebp,esp
   0x08048447 <+3>:	sub    esp,0x18
   0x0804844a <+6>:	mov    eax,DWORD PTR [ebp+0x8]
   0x0804844d <+9>:	mov    DWORD PTR [esp],eax
   0x08048450 <+12>:	call   0x8048340 <printf@plt>
   0x08048455 <+17>:	leave  
   0x08048456 <+18>:	ret    
End of assembler dump.

```

### **Línea 0: `push   ebp`**
```asm
0x08048444 <+0>:	push   ebp
```
- Guarda el valor de EBP (función `n`) en lo alto de stack: [ebp + 0x00]. Guarda el suelo para no perderlo.
- El ESP se mueve hacia abajo 4 bytes.

### **Líneas 1 y 3 **
```asm
0x08048445 <+1>:	mov    ebp,esp
0x08048447 <+3>:	sub    esp,0x18
```
- 1. Copia el nuevo ESP del stack para tener un nuevo EBP para la función `p`
- 2. Reserva en el stack 24 bytes (0x18) `buffer`

### **Líneas: 6, 9 y 12**
```asm
0x0804844a <+6>:	mov    eax,DWORD PTR [ebp+0x8]
0x0804844d <+9>:	mov    DWORD PTR [esp],eax
0x08048450 <+12>:	call   0x8048340 <printf@plt>
```
- 1. Guarda el argumento recibido de la función `p` en EAX
- 2. Mueve EAX a lo alto del stack.
- 3. Llama a la función `printf()` (muestra por pantalla lo que hay guardado en en el stack `DWORD PTR [esp]`)

**Vunerabilidad detectada:** Se está llamanda a `printf(buffer)` en lugar de `printf("%s", buffer)`. Esto se conoce como **Format String Vulnerability**. NO necesitamos romper el buffer por tamaño **(buffer overflow)**, sino explotar como print interpreta los datos.

### **Resumen del Flujo de Ataque para el Nivel 4:**

1. **Lectura**: `main` solo alinea el stack y llama a `n`,`n` lee con `fgets` pero realiza la impresión en `p`.
2. **Entrada**: `fgets` limita la lectura, impidiendo un *Buffer Overflow* tradicional.
3. **Vulnerabilidad**: `printf` recibe nuestro buffer sin un string de formato fijo.
4. **Explotación**: Modificar la variable global en `0x8049810` para que valga `0x1025544`.