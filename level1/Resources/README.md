# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level1**

```
Dump of assembler code for function main:
   0x08048480 <+0>:     push   ebp
   0x08048481 <+1>:	    mov    ebp,esp
   0x08048483 <+3>:	    and    esp,0xfffffff0
   0x08048486 <+6>:	    sub    esp,0x50
   0x08048489 <+9>:	    lea    eax,[esp+0x10]
   0x0804848d <+13>:	  mov    DWORD PTR [esp],eax
   0x08048490 <+16>:	  call   0x8048340 <gets@plt>
   0x08048495 <+21>:	  leave  
   0x08048496 <+22>:	  ret    
End of assembler dump.

```

## **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Línea 0: `push ebp`**
```
0x08048480 <+0>:     push   ebp
```
- Guarda el valor de EBP de la CPU en el stack: [ebp + 0x00]. Guarda el "suelo" anterior para no perderlo.
- El ESP se mueve 4 bytes hacia abajo.

### **Línea 1: `mov    ebp,esp` **
```
0x08048481 <+1>:	    mov    ebp,esp
```
- Copia el nuevo `esp` del stack para tener un nuevo registro para la función `main`.

### **Línea 3: `and    esp,0xfffffff0` **
```
0x08048483 <+3>:	    and    esp,0xfffffff0
```
- Alinea el stack a múltiplo de 16 bytes.
- Los últimos 4 bits de ESP se ponen a 0 (ej: 0xbffff7c9 → 0xbffff7c0)

### **Línea 6: `sub    esp,0x50` **
```
0x08048486 <+6>:	    sub    esp,0x50
```
- Reserva espacio para variables locales en el stack
- 80 bytes (0x50) para uso de la función `main()` 


### **Línea 9: `lea    eax,[esp+0x10]` **
```
0x08048489 <+9>:	    lea    eax,[esp+0x10]
```
- Calcula dónde empieza el buffer para  `gets()`.
- El buffer no empieza al principio de los 80 bytes, sino a 16 bytes de distancia del tope (esp).
- Espacio disponible real antes del EBP: 80−16=64 bytes. (0x50 = 80 y 0x10 = 16)
- `eax = &stack[esp+0x10]` (dirección donde guardamos `gets()`)


### **Línea 13: `mov    DWORD PTR [esp],eax` **
```
0x0804848d <+13>:	mov    DWORD PTR [esp],eax
```
- Guarda eax en el tope del stack


### **Línea 16: `call   0x8048340 <gets@plt>` **
```
0x08048490 <+16>:	call   0x8048340 <gets@plt>
```
- LLama a `get(buffer)` para leer desde `stdin`
- eax = a lo que ha leido `stdin`

### **Línea 21: `leave`**
```
0x08048495 <+21>:	leave  
```
- **Equivalente a**:
  ```
  mov esp,ebp    ; Restaura ESP
  pop ebp        ; Restaura EBP antiguo
  ```
- Deshace el stack frame de la función

### **Línea 22: `ret` **
```
0x08048496 <+22>:	ret
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`
