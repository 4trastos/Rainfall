# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level3**

```asm
DDump of assembler code for function main:
   0x0804851a <+0>:	push   ebp
   0x0804851b <+1>:	mov    ebp,esp
   0x0804851d <+3>:	and    esp,0xfffffff0
   0x08048520 <+6>:	call   0x80484a4 <v>
   0x08048525 <+11>  leave  
   0x08048526 <+12>: ret    
End of assembler dump.

```

# **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Línea 0: `push   ebp`**
```asm
0x0804851a <+0>:	push   ebp
```
- Guarda el valor de EBP (CPU) en lo alto del stack: [ebp + 0x00]. Guarda el suelo anterior para no perderlo.
- El ESP se mueve 4 bytes hacia abajo.

### **Línea 1: `mov    ebp,esp` **
```asm
0x0804851b <+1>:	mov    ebp,esp
```
- Copia el nuevo ESP el stack para tener un nuevo registro para la función `main`.

### **Línea 3: `and    esp,0xfffffff0` **
```asm
0x0804851d <+3>:	and    esp,0xfffffff0
```
- Alinea el stack a multiplo de 16 bytes.
- Los últimos 4 bits de ESP se ponen a 0 (ej: 0xbffff7c9 → 0xbffff7c0)

### **Línea 6: `call   0x80484a4 <v>` **
```asm
0x08048520 <+6>:	call   0x80484a4 <v>
```
- LLama a la función `v`.

### **Línea 11: `leave`**
```asm
0x0804854a <+11>:	leave   
```
- **Equivalente a**:
```asm
mov esp,ebp    ; Restaura ESP
pop ebp        ; Restaura EBP antiguo
```
- Deshace el stack frame de la función

### **Línea 12: `ret` **
```asm
0x0804854b <+12>:	ret 
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`

# **SECCIÓN 2: ANALÁSIS DE LA FUNCIÓN `v`**

```asm
Dump of assembler code for function v:
   0x080484a4 <+0>:	   push   ebp
   0x080484a5 <+1>:	   mov    ebp,esp
   0x080484a7 <+3>:	   sub    esp,0x218                     // -> 538 bytes (0x218) 
   0x080484ad <+9>:	   mov    eax,ds:0x8049860
   0x080484b2 <+14>:	   mov    DWORD PTR [esp+0x8],eax
   0x080484b6 <+18>:	   mov    DWORD PTR [esp+0x4],0x200
   0x080484be <+26>:	   lea    eax,[ebp-0x208]
   0x080484c4 <+32>:	   mov    DWORD PTR [esp],eax
   0x080484c7 <+35>:	   call   0x80483a0 <fgets@plt>
   0x080484cc <+40>:	   lea    eax,[ebp-0x208]
   0x080484d2 <+46>:	   mov    DWORD PTR [esp],eax
   0x080484d5 <+49>:	   call   0x8048390 <printf@plt>
   0x080484da <+54>:	   mov    eax,ds:0x804988c
   0x080484df <+59>:	   cmp    eax,0x40
   0x080484e2 <+62>:	   jne    0x8048518 <v+116>
   0x080484e4 <+64>:	   mov    eax,ds:0x8049880
   0x080484e9 <+69>:	   mov    edx,eax
   0x080484eb <+71>:	   mov    eax,0x8048600
   0x080484f0 <+76>:	   mov    DWORD PTR [esp+0xc],edx
   0x080484f4 <+80>:	   mov    DWORD PTR [esp+0x8],0xc
   0x080484fc <+88>:	   mov    DWORD PTR [esp+0x4],0x1
   0x08048504 <+96>:	   mov    DWORD PTR [esp],eax
   0x08048507 <+99>:	   call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:	mov    DWORD PTR [esp],0x804860d
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave  
   0x08048519 <+117>:	ret    
End of assembler dump.
```

### **Línea 0: `push ebp`**
```asm
0x080484a4 <+0>:	push   ebp
```
- Guarda en valor de EBP (`main`) en el stack: [ebp + 0x00]. Guarda el suelo paa no perderlo.
- El ESP se mueve hacia abajo 4 bytes.

### **Línea 1: `mov    ebp,esp`**
```asm
0x080484a5 <+1>:	mov    ebp,esp
```
- Copia el nuevo ESP del stack para tener un nuevo registro para la función `v`.

### **Línea 3: `sub    esp,0x218`**
```asm
0x080484a7 <+3>:	sub    esp,0x218
```
- Reserva 538 bytes (0x218) para el uso de la función `v`.

### **Línea 9: `mov    eax,ds:0x8049860`**
```asm
0x080484ad <+9>:	mov    eax,ds:0x8049860
```
- Copia el contenido de la dirección `0x8049860` en EAX
- `ds:` Prefijo de segmento de datos.
- Es probable que en `0x8049860` se encuentre: `stderror` (FILE*)

## **Línea 14, 18, 26, 32 y 35: `mov    DWORD PTR [esp+0x8],eax` - `DWORD PTR [esp+0x4],0x200` - `lea    eax,[ebp-0x208]`- `mov    DWORD PTR [esp],eax` - `0x080484c7 <+35>:	   call   0x80483a0 <fgets@plt>]`**

```asm
0x080484b2 <+14>:	   mov    DWORD PTR [esp+0x8],eax
0x080484b6 <+18>:	   mov    DWORD PTR [esp+0x4],0x200
0x080484be <+26>:	   lea    eax,[ebp-0x208]
0x080484c4 <+32>:	   mov    DWORD PTR [esp],eax
0x080484c7 <+35>:	   call   0x80483a0 <fgets@plt>
```
- 1. ESP mueve 8 bytes hacia abajo y guarda el registro EAX
- 2. ESP mueve 4 bytes hacia abajo y guarda el limmite de lectura del buffer (512 bytes)
- 3. Calcula donde se encuentra la salida del buffer y lo guarda en EAX. (el buffer completo). 
- 4. Mueve EAX al tope del stack
- 5. Llama a la función `fgets()`

## **Línea 46, 49, 54: `lea    eax,[ebp-0x208]` - `mov    DWORD PTR [esp],eax` - `call   0x8048390 <printf@plt>`**
```asm
0x080484cc <+40>:	   lea    eax,[ebp-0x208]
0x080484d2 <+46>:	   mov    DWORD PTR [esp],eax
0x080484d5 <+49>:	   call   0x8048390 <printf@plt>
```
- 1. Calcula donde se encuentra el buffer leido
- 2. Mueve EAX (la dirección del buffer) al tope del stack. **COMO PRIMER ARGUMENTO DE `printf()`**
- 3. Llama a la función `printf()` (muestra por pantalla lo que ha recodigo por el `stdin`)

**Vulnerabilidad detectada:** Se está llamando a `printf(buffer)` en lugar de `printf("%s", buffer)`. Esto se conoce como **Format String Vulnerability**. No necesitamos "romper" el buffer por tamaño **(buffer overflow)**, sino explotar cómo printf interpreta los datos.

## **Línea: 54, 59, 60: `mov    eax,ds:0x804988c` - `cmp    eax,0x40` - `jne    0x8048518 <v+116>`**
```asm
0x080484da <+54>:	   mov    eax,ds:0x804988c
0x080484df <+59>:	   cmp    eax,0x40
0x080484e2 <+62>:	   jne    0x8048518 <v+116>
```
- 1. Copia el contienido de la dirección `0x804988c`. `ds:` Prefijo de segmento de datos.
- 2. Compara EAX ceon el valor Hexadecimal 64.
- 3. Si el valos coincide no salta a `0x8048518 <v+116>`.

Aquí tienes la finalización del análisis línea por línea de la función `v`, manteniendo el formato y la estructura que ya has definido:

---

### **Línea 64 a 99: Configuración de `fwrite()**`

```asm
0x080484e4 <+64>:    mov    eax,ds:0x8049880
0x080484e9 <+69>:    mov    edx,eax
0x080484eb <+71>:    mov    eax,0x8048600
0x080484f0 <+76>:    mov    DWORD PTR [esp+0xc],edx
0x080484f4 <+80>:    mov    DWORD PTR [esp+0x8],0xc
0x080484fc <+88>:    mov    DWORD PTR [esp+0x4],0x1
0x08048504 <+96>:    mov    DWORD PTR [esp],eax
0x08048507 <+99>:    call   0x80483b0 <fwrite@plt>

```

* **Acción**: Estas líneas preparan los 4 argumentos necesarios para la función `fwrite(ptr, size, nmemb, stream)`.
* **Argumento 4 (stream)**: Carga el puntero `stdout` desde `0x8049880`.
* **Argumento 3 (nmemb)**: Se pone a `0xc` (12 en decimal).
* **Argumento 2 (size)**: Se pone a `0x1`.
* **Argumento 1 (ptr)**: Carga la dirección `0x8048600` (donde reside el string `"Wait what?!\n"`).
* **Resultado**: Imprime el mensaje de éxito en la consola si la comparación anterior fue correcta.

### **Línea 104 y 111: El Backdoor - `system("/bin/sh")**`

```asm
0x0804850c <+104>:   mov    DWORD PTR [esp],0x804860d
0x08048513 <+111>:   call   0x80483c0 <system@plt>

```

* **Línea 104**: Mueve al tope del stack la dirección `0x804860d`. Esta dirección de memoria contiene el string `"/bin/sh"`.
* **Línea 111**: Ejecuta la llamada al sistema. Al ser un binario **SUID**, esto nos otorga una shell con los privilegios del propietario del nivel superior.

### **Línea 116 y 117: Epílogo de la Función**

```asm
0x08048518 <+116>:   leave  
0x08048519 <+117>:   ret    

```

* **`leave`**: Restaura los registros `ESP` y `EBP` al estado en que estaban antes de entrar en `v`, limpiando el stack frame de 538 bytes.
* **`ret`**: Saca la dirección de retorno del stack y devuelve el control a `main`.

---

### **Resumen del Flujo de Ataque para el Nivel 3:**

1. **Entrada**: `fgets` limita la lectura, impidiendo un *Buffer Overflow* tradicional.
2. **Vulnerabilidad**: `printf` recibe nuestro buffer sin un string de formato fijo.
3. **Explotación**: Usamos el modificador `%n` para escribir en la memoria.
4. **Target**: Apuntamos a la dirección `0x804988c` (variable `m`).
5. **Valor**: Debemos imprimir exactamente **64 caracteres** antes del `%n` para que el valor de `m` sea `0x40`.
6. **Resultado**: El `cmp` resulta positivo, no se ejecuta el `jne`, y el programa nos regala la shell mediante `system`.