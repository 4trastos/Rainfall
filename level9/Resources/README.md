# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level9**

```asm
Dump of assembler code for function main:
   0x080485f4 <+0>:	    push   ebp
   0x080485f5 <+1>:	    mov    ebp,esp
   0x080485f7 <+3>:	    push   ebx
   0x080485f8 <+4>:	    and    esp,0xfffffff0
   0x080485fb <+7>:	    sub    esp,0x20
   0x080485fe <+10>:	cmp    DWORD PTR [ebp+0x8],0x1
   0x08048602 <+14>:	jg     0x8048610 <main+28>
   0x08048604 <+16>:	mov    DWORD PTR [esp],0x1
   0x0804860b <+23>:	call   0x80484f0 <_exit@plt>
   0x08048610 <+28>:	mov    DWORD PTR [esp],0x6c
   0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>
   0x0804861c <+40>:	mov    ebx,eax
   0x0804861e <+42>:	mov    DWORD PTR [esp+0x4],0x5
   0x08048626 <+50>:	mov    DWORD PTR [esp],ebx
   0x08048629 <+53>:	call   0x80486f6 <_ZN1NC2Ei>
   0x0804862e <+58>:	mov    DWORD PTR [esp+0x1c],ebx
   0x08048632 <+62>:	mov    DWORD PTR [esp],0x6c
   0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>
   0x0804863e <+74>:	mov    ebx,eax
   0x08048640 <+76>:	mov    DWORD PTR [esp+0x4],0x6
   0x08048648 <+84>:	mov    DWORD PTR [esp],ebx
   0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>
   0x08048650 <+92>:	mov    DWORD PTR [esp+0x18],ebx
   0x08048654 <+96>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048658 <+100>:	mov    DWORD PTR [esp+0x14],eax
   0x0804865c <+104>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048660 <+108>:	mov    DWORD PTR [esp+0x10],eax
   0x08048664 <+112>:	mov    eax,DWORD PTR [ebp+0xc]
   0x08048667 <+115>:	add    eax,0x4
   0x0804866a <+118>:	mov    eax,DWORD PTR [eax]
   0x0804866c <+120>:	mov    DWORD PTR [esp+0x4],eax
   0x08048670 <+124>:	mov    eax,DWORD PTR [esp+0x14]
   0x08048674 <+128>:	mov    DWORD PTR [esp],eax
   0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
   0x0804867c <+136>:	mov    eax,DWORD PTR [esp+0x10]
   0x08048680 <+140>:	mov    eax,DWORD PTR [eax]
   0x08048682 <+142>:	mov    edx,DWORD PTR [eax]
   0x08048684 <+144>:	mov    eax,DWORD PTR [esp+0x14]
   0x08048688 <+148>:	mov    DWORD PTR [esp+0x4],eax
   0x0804868c <+152>:	mov    eax,DWORD PTR [esp+0x10]
   0x08048690 <+156>:	mov    DWORD PTR [esp],eax
   0x08048693 <+159>:	call   edx
   0x08048695 <+161>:	mov    ebx,DWORD PTR [ebp-0x4]
   0x08048698 <+164>:	leave  
   0x08048699 <+165>:	ret    
End of assembler dump.
```

### **Líneas 0, 1, 3, 4, 7, 10 y 14:**
```asm
0x080485f4 <+0>:	push   ebp
0x080485f5 <+1>:	mov    ebp,esp
0x080485f7 <+3>:	push   ebx
0x080485f8 <+4>:	and    esp,0xfffffff0
0x080485fb <+7>:	sub    esp,0x20
0x080485fe <+10>:	cmp    DWORD PTR [ebp+0x8],0x1
0x08048602 <+14>:	jg     0x8048610 <main+28>
0x08048604 <+16>:	mov    DWORD PTR [esp],0x1
0x0804860b <+23>:	call   0x80484f0 <_exit@plt>
```

1. <+0>: Guarda en el stack el valor de EBP (CPU): `[esp + 0x00]` y ESP se desplaza 4 bytes hacia abajo
2. <+1>: Copia en EBP el nuevo ESP de stack para la función `main`
3. <+3>: Guarda en el stack el registro de EBX para usarlo más tarde `[esp + 0x04]` y ESP se desplaza 4 bytes hacia abajo
4. <+4>: Alinea el stack como múltiplo de 16, (os 4 últimos bytes de ESP se ponen a 0)
5. <+7>: Reserva (desplaza) 32 bytes (0x20). Espacio para el compilador en el stack de `main()` para organizar las variables locales y los argumentos de las funciones que `main` va a llamar.
6. <+10>: Desplaza 8 bytes el puntero (donde debería de estar `argc`) y compara el número de argumentos con 1.
7. <+14>: `*JG*` (Jump if Greate) Si `argc` es mayor que 1 ha recibido argumentos y salta a la línea <+28>
8. <+16>: Si solo hya un argumento desplaza el puntero al inicio de stack.
9. <+23>: Llama a la función `exit` y el programa termina.

### **Líneas 28 y 35:**
```asm
0x08048610 <+28>:	mov    DWORD PTR [esp],0x6c
0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>
```

1. <+28>: Pone el 108 (0x6c) al inicio del stack. Tamaño en bytes.
2. <+35>: Llama a `operator new(unsigned int)`. En C++ es lo mismo que el `new`. Es como un `malloc`, pero para objetos. Está reservando `108 bytes` en el Heap para crear una instancia de una clase (el binario llama calse `N`).:
```bash
(gdb) info functions 
All defined functions:

Non-debugging symbols:
...
0x080486f6  N::N(int)
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)
0x0804873a  N::operator+(N&)
0x0804874e  N::operator-(N&)
...
```

### **Líneas 40, 42, 50, 53 y 58:**
```asm
0x0804861c <+40>:	mov    ebx,eax
0x0804861e <+42>:	mov    DWORD PTR [esp+0x4],0x5
0x08048626 <+50>:	mov    DWORD PTR [esp],ebx
0x08048629 <+53>:	call   0x80486f6 <_ZN1NC2Ei>
0x0804862e <+58>:	mov    DWORD PTR [esp+0x1c],ebx
```

1. <+40>: Guarda la dirección del nuevo objeto (EAX) en el registro EBX.
2. <+42>: Pone un 5 (0x05) como segundo argumento. `[esp+0x4]`
3. <+50>: Pone la dirección del objeto como primer argumento. `[esp]`
4. <+53>: Llama a `N::N(int)` (`0x80486f6`). Es el Constructor de la clase `N`. Básicamente inicializa el objeto con el valor 5.
```bash
gdb) info functions 
All defined functions:

Non-debugging symbols:
...
0x080486f6  N::N(int)         ←     0x80486f6
...
```
5. <+58>: Guarda el puntero de este objeto en `[esp+0x1c]`.

### **Líneas 62, 69, 74, 76, 84 y 87:**
```asm
0x08048632 <+62>:	mov    DWORD PTR [esp],0x6c
0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>
0x0804863e <+74>:	mov    ebx,eax
0x08048640 <+76>:	mov    DWORD PTR [esp+0x4],0x6
0x08048648 <+84>:	mov    DWORD PTR [esp],ebx
0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>
```

1. <+62>: Pone 108 (0x6c) en el stack. Tamaño en bytes.
2. <+69>: Hace de nuevo un `new` reservando 108 bytes.
3. <+74>: Guarda la dirección del nuevo objeto en EBX
4. 