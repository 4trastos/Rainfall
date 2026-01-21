# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level2**

```
Dump of assembler code for function main:
   0x0804853f <+0>:	    push   ebp
   0x08048540 <+1>:	    mov    ebp,esp
   0x08048542 <+3>:	    and    esp,0xfffffff0
   0x08048545 <+6>:	    call   0x80484d4 <p>
   0x0804854a <+11>:	leave  
   0x0804854b <+12>:	ret    
End of assembler dump.

```

# **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Línea 0: `push ebp`**
```
0x0804853f <+0>:	    push   ebp
```
- Guarda el valor de EBP de la CPU en el stack: [ebp + 0x00]. Guarda el "suelo" anterior para no perderlo.
- El ESP se mueve 4 bytes hacia abajo.

### **Línea 1: `mov    ebp,esp` **
```
0x08048540 <+1>:	    mov    ebp,esp
```
- Copia el nuevo `esp` del stack para tener un nuevo registro para la función `main`.

### **Línea 3: `and    esp,0xfffffff0` **
```
0x08048542 <+3>:	    and    esp,0xfffffff0
```
- Alinea el stack a múltiplo de 16 bytes.
- Los últimos 4 bits de ESP se ponen a 0 (ej: 0xbffff7c9 → 0xbffff7c0)

### **Línea 6: `call   0x80484d4 <p>` **
```
0x08048545 <+6>:	    call   0x80484d4 <p>
```
- Llama a la función `p` para leer desde e `stdin` y copiar con `strdup()`  
- eax = a lo que ha duplicado `strdup()` 

### **Línea 11: `leave`**
```
0x0804854a <+11>:	leave   
```
- **Equivalente a**:
  ```
  mov esp,ebp    ; Restaura ESP
  pop ebp        ; Restaura EBP antiguo
  ```
- Deshace el stack frame de la función

### **Línea 12: `ret` **
```
0x0804854b <+12>:	ret 
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`


# **SECCIÓN 2: ANALÁSIS DE LA FUNCIÓN `p**

```
Dump of assembler code for function p:
   0x080484d4 <+0>:     push   ebp
   0x080484d5 <+1>:	    mov    ebp,esp
   0x080484d7 <+3>:	    sub    esp,0x68
   0x080484da <+6>:	    mov    eax,ds:0x8049860
   0x080484df <+11>:	mov    DWORD PTR [esp],eax
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:	lea    eax,[ebp-0x4c]
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>
   0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]
   0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:	and    eax,0xb0000000
   0x08048500 <+44>:	cmp    eax,0xb0000000
   0x08048505 <+49>:	jne    0x8048527 <p+83>
   0x08048507 <+51>:	mov    eax,0x8048620
   0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:	lea    eax,[ebp-0x4c]
   0x0804852a <+86>:	mov    DWORD PTR [esp],eax
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>
   0x08048532 <+94>:	lea    eax,[ebp-0x4c]
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:	leave  
   0x0804853e <+106>:	ret    
End of assembler dump.
```

### **Línea 0: `push ebp`**
```
0x080484d4 <+0>:     push   ebp
```
- Guarda el valor de EBP de `main` en el stack: [ebp + 0x00]. Guarda el suelo anterior para no perderlo
- El ESP se mueve 4 bytes hacia abajo.

### **Línea 1: `mov    ebp,esp`**
```
0x080484d5 <+1>:	    mov    ebp,esp
```
- Copia el nuevo `esp` del stack para tener un nuevo registro para la función `p`

### **Línea 3: `sub    esp,0x68`**
```
0x080484d7 <+3>:	    sub    esp,0x68
```
- Reversa espacio para variables locales en el stack
- 104 bytes (0x68) para el uso de la función `p`

### **Línea 6: `mov    eax,ds:0x8049860`**
```
0x080484da <+6>:	    mov    eax,ds:0x8049860
```
- Copia el contenido de la direccíon 0x8049860 en EAX
- `ds:` Prefijo de segmento de datos
- ¿Qué hay en 0x8049860?: Probablemte `stderr` (FILE*)

### **Línea 11: `mov    DWORD PTR [esp],eax`**
```
0x080484df <+11>:	mov    DWORD PTR [esp],eax
```
- Guarda EAX en lo alto del stack.

### **Línea 14: `call   0x80483b0 <fflush@plt>`**
```
0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>
```
- Llama a la función `fflush()` (int fflush(FILE *_Nullable stream)).
- eax = Si se ha completado correctamente la escritura en el buffer o no.

### **Línea 19: `lea    eax,[ebp-0x4c]`**
```
0x080484e7 <+19>:	lea    eax,[ebp-0x4c]
```
- Calcula donde se encuentra la variable buffer que es donde almacenará `gets()`. 
- Está a 78 bytes (0x4c = 78) del inicio de la función `p`
- eax = buffer.

### **Línea 22: `mov    DWORD PTR [esp],eax`**
```
0x080484ea <+22>:	mov    DWORD PTR [esp],eax
```
- Guarda EAX en el tope del stack.

### **Línea 25: `call   0x80483c0 <gets@plt>`**
```
0x080484ed <+25>:	call   0x80483c0 <gets@plt>
```
- Llama a `gets()` para leer desde `stdin`
- eax = Lo que ha leido `stdin`

### **Línea 30: `mov    eax,DWORD PTR [ebp+0x4]`**
```
0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]
```
- Guarda en eax la dirección de retorno cuando termine la función `p`

### **Línea 33: `mov    DWORD PTR [ebp-0xc],eax`**
```
0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax
```
- Guarda la dirección de retorno inspeccionada en una variable local `([ebp-0xc])` para procesarla después con el `AND` y el `CMP`.

### **Línea 36: `mov    eax,DWORD PTR [ebp-0xc]`**
```
0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
```
- Guarda el contenido de buffer en eax;

### **Línea 39 y 44: El "Filtro de Seguridad (El Detector de Inyecciones)"**
```
0x080484fb <+39>:    and    eax,0xb0000000
0x08048500 <+44>:    cmp    eax,0xb0000000
0x08048505 <+49>:    jne    0x8048527 <p+83>
```
- El programa verifica si el EIP ha sido alterado para apuntar al Stack:
    a). Toma la dirección de retorno (EIP).
    b). Le aplica una máscara AND con `0xb0000000`.
    c). Compara si el resultado es `0xb0000000`.

- Si `(EIP & 0xb0000000) == 0xb0000000`, el programa asume un ataque y aborta. Saltando a la línea 83 => `0x08048527 <+83>:	lea    eax,[ebp-0x4c]
- Esto impide ejecutar Shellcode directamente desde el buffer del Stack.

### **Línea 100: `call   0x80483e0 <strdup@plt>`**
```
0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
```
- Aquí está la brecha. El programa hace un `strdup` del buffer.
- `strdup` reserva memoria en el Heap.
- Las direcciones del Heap no empiezan por `0xb` (suelen empezar por `0x08`).

### **Línea 105: `leave`**
```
0x0804853d <+105>:	leave   
```
- **Equivalente a**:
  ```
  mov esp,ebp    ; Restaura ESP
  pop ebp        ; Restaura EBP antiguo
  ```
- Deshace el stack frame de la función

### **Línea 106: `ret` **
```
0x0804853e <+106>:	ret  
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí