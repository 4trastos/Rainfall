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
- 