# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level6**

```asm
Dump of assembler code for function main:
   0x0804847c <+0>:	   push   ebp
   0x0804847d <+1>:	   mov    ebp,esp
   0x0804847f <+3>:	   and    esp,0xfffffff0
   0x08048482 <+6>:	   sub    esp,0x20
   0x08048485 <+9>:	   mov    DWORD PTR [esp],0x40
   0x0804848c <+16>:	   call   0x8048350 <malloc@plt>
   0x08048491 <+21>:	   mov    DWORD PTR [esp+0x1c],eax
   0x08048495 <+25>:	   mov    DWORD PTR [esp],0x4
   0x0804849c <+32>:	   call   0x8048350 <malloc@plt>
   0x080484a1 <+37>:	   mov    DWORD PTR [esp+0x18],eax
   0x080484a5 <+41>:	   mov    edx,0x8048468
   0x080484aa <+46>:	   mov    eax,DWORD PTR [esp+0x18]
   0x080484ae <+50>:	   mov    DWORD PTR [eax],edx
   0x080484b0 <+52>:	   mov    eax,DWORD PTR [ebp+0xc]
   0x080484b3 <+55>:	   add    eax,0x4
   0x080484b6 <+58>:	   mov    eax,DWORD PTR [eax]
   0x080484b8 <+60>:	   mov    edx,eax
   0x080484ba <+62>:	   mov    eax,DWORD PTR [esp+0x1c]
   0x080484be <+66>:	   mov    DWORD PTR [esp+0x4],edx
   0x080484c2 <+70>:	   mov    DWORD PTR [esp],eax
   0x080484c5 <+73>:	   call   0x8048340 <strcpy@plt>
   0x080484ca <+78>:	   mov    eax,DWORD PTR [esp+0x18]
   0x080484ce <+82>:	   mov    eax,DWORD PTR [eax]
   0x080484d0 <+84>:	   call   eax
   0x080484d2 <+86>:	   leave  
   0x080484d3 <+87>:	   ret    
End of assembler dump.
```

# **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Líneas 0, 1, 3y 6:**
```asm
0x0804847c <+0>:	push   ebp
0x0804847d <+1>:	mov    ebp,esp
0x0804847f <+3>:	and    esp,0xfffffff0
0x08048482 <+6>:	sub    esp,0x20
```
1. Guarda el valor de EBP (CPU) en lo alto del stack: [ebp + 0x00]. Guarda el suelo para no perderlo y ESP se mueve 4 bytes hacia abajo.
2. Copia el nuevo ESP del stack en el registro EBP como nuevo para la función `main`
3. Alinea el stack como multiplo de 16 bytes. (los últimos 4 bytes de ESP se ponen a 0).
4. Reserva (desplaza) 32 bytes (0x20) Espacio que el compilador reserva en el stack de `main` para organizar la varibales locales y los argumentos de las funciones que va a llamar: `malloc`, `strcpy`, etc.

### **Líneas 9, 16, 21, 25, 32 y 37:**
```asm
0x08048485 <+9>:	mov    DWORD PTR [esp],0x40
0x0804848c <+16>:	call   0x8048350 <malloc@plt>
0x08048491 <+21>:	mov    DWORD PTR [esp+0x1c],eax
0x08048495 <+25>:	mov    DWORD PTR [esp],0x4
0x0804849c <+32>:	call   0x8048350 <malloc@plt>
0x080484a1 <+37>:	mov    DWORD PTR [esp+0x18],eax
```
1. Se pone el valor `64 (0x40)` en el tope del stack como argumento para la función `malloc`.
2. llama a la función `malloc` y reserva los 64 bytes de antes para un `buffer` (`char *`).
3. Guarda el valor de EAX en **[esp+0x1c]** (`buffer = malloc(64)`). Guardado en el `heap`.
4. Desplaza el ESP 4 bytes para usarlos con el segundo `malloc`
5. Llama a la función `malloc` y reserva espacio para un puntero. Los 4 bytes de antes.
6. Guarda el puntero en **[esp+0x18]**. Lo guarda en el `heap`.

### **Líneas 41, 46 y 50:**
```asm
0x080484a5 <+41>:	mov    edx,0x8048468             ; EDX = &m()
0x080484aa <+46>:	mov    eax,DWORD PTR [esp+0x18]  ; EAX = puntero del heap (segundo malloc)
0x080484ae <+50>:	mov    DWORD PTR [eax],edx       ; *EAX = &m()
```
1. Carga la dirección `0x8048468` en EDX: 
- Vemos que en esa dirección con el comando  `objdump` que está la función `m`. 
```bash
level6@RainFall:~$ objdump -d ./level6 | grep 08048468
08048468 <m>:              ⟸
level6@RainFall:~$
``` 
- Desensamblamos la función `m` con `objdump`:
```bash
level6@RainFall:~$ objdump -d ./level6
08048468 <m>:
 8048468:	55                   	push   %ebp
 8048469:	89 e5                	mov    %esp,%ebp
 804846b:	83 ec 18             	sub    $0x18,%esp
 804846e:	c7 04 24 d1 85 04 08 	movl   $0x80485d1,(%esp)      ⟸ 
 8048475:	e8 e6 fe ff ff       	call   8048360 <puts@plt>
 804847a:	c9                   	leave  
 804847b:	c3                   	ret
```
- En `0x80485d1` se encuentra la dirección de la cadena que imprime `Nope` con el syscall `puts`. Podemos verla con GDB:
```bash
level6@RainFall:~$ gdb ./level6
(gdb) x/s 0x80485d1
0x80485d1:	 "Nope"
(gdb) quit
level6@RainFall:~$ 
```
- **¡IMPORTANTE!:** Guarda esa dirección `(edx)` dentro del segundo `malloc` que acabamos de crear.
- Esto significa que el segundo `malloc` contiene la dirección de la función que el programa espera ejecutar.

2. Escribe la dirección de `m()` dentro del heap, en los 4 bytes apuntados por el segundo `malloc`
3. Escribe el contenido de EDX (la dirección de la función m) en los 4 bytes apuntados por EAX (el puntero almacenado en el heap).

### **Líneas 52, 55, 58, 60, 62, 70 y 73:**
```asm
0x080484b0 <+52>:	mov    eax,DWORD PTR [ebp+0xc]
0x080484b3 <+55>:	add    eax,0x4
0x080484b6 <+58>:	mov    eax,DWORD PTR [eax]
0x080484b8 <+60>:	mov    edx,eax
0x080484ba <+62>:	mov    eax,DWORD PTR [esp+0x1c]
0x080484be <+66>:	mov    DWORD PTR [esp+0x4],edx
0x080484c2 <+70>:	mov    DWORD PTR [esp],eax
0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>
```
1. Copia en EAX la direcciòn de [ebp+0xc] `argv`.     
2. Mueve 4 bytes en EAX pare apuntar al `argv[1]`.
3. char *src = argv[1]
4. edx = src
5. eax = ptr1 (buffer de 64 bytes)
6. 2º argumento = src
7. 1º argumento = dest
8. Llama a `strcpy`=> **strcpy(ptr1, argv[1])**

**Exploit detectado:** Podemos usar el **Heap-based Buffer Overflow**. Copia el primer argumento del programa (argv[1]) dentro del `buffer` de 64 bytes. No hay límite. `strcpy` copia hasta encontrar un `\0`.
Si argv[1] tiene más de 64 bytes, desborda el heap.

### **Lineas 78, 82 y 84:**
```asm
0x080484ca <+78>:	   mov    eax,DWORD PTR [esp+0x18]
0x080484ce <+82>:	   mov    eax,DWORD PTR [eax]
0x080484d0 <+84>:	   call   eax
```
**La llamada a la función**:
1. eax = ptr2
2. eax = *ptr2 (dirección de función)
3. llama a donde apunte ptr2

**Explotación**: Si logramos sobreescribir `*ptr2` con otra dirección, cuando haga `call eax` no llamará a `m()`, sino a la función que le pongamos.

**¿Qué dirección ponemos?**
1. Buscamos si hay mas funciones en el binario y encontramos la función `n()`:
```bash
level6@RainFall:~$ gdb ./level6
(gdb) set disassembly-flavor intel
(gdb) info functions
0x08048454  n           ⟸
0x08048468  m
0x0804847c  main
```
2. Desensamblamos la función `n()` y veoms que argumento recibe `system()`:
```bash
(gdb) disas n
Dump of assembler code for function n:
   0x08048454 <+0>:	push   ebp
   0x08048455 <+1>:	mov    ebp,esp
   0x08048457 <+3>:	sub    esp,0x18
   0x0804845a <+6>:	mov    DWORD PTR [esp],0x80485b0    ⟸
   0x08048461 <+13>:	call   0x8048370 <system@plt>
   0x08048466 <+18>:	leave  
   0x08048467 <+19>:	ret    
End of assembler dump.
(gdb) x/s 0x80485b0
0x80485b0:	 "/bin/cat /home/user/level7/.pass"
```
3. **dirección para exploit**: `0x08048454` `n()` llama a `system("/bin/cat /home/user/level7/.pass")`.

### **Linea 86: `leave`**
```asm
0x080484d2 <+86>:	   leave  
```
- **Equivale a:**
```asm
mov   esp,ebp  ; Restaurar ESP
pop   ebp      ; Restaurar EBP antiguo
```

### **Línea 87: `ret`**
```asm
0x080484d3 <+87>:	   ret 
```
- **Acción**: Return - Saca la dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución del `main()`.

### **Resumen del Flujo de Ataque para el Nivel 6**

1. **Lectura:** El programa reserva dos bloques en el `heap`. El primero en un `buffer` de 64 bytes y el segundo es un puntero que aputa a la función `m()` (que imprime `Nope`).
2. **Vulnerabilidad:** Se utiliza `strcpy` para copiar argv[1] al primer buffer sin tener en cuenta el tamaño. Esto permite **Heap-based Buffer Overflow**. ¿Por qué el desbordamineto llega al punteero?. En el `heap` cuando se hacen dos malloc seguidos el sistema suele colocarlo uno tras otro.
3. **Objetivo:** Sobrescribir el contenido del segundo bloque (el puntero de función) para que en el lugar de contener la dirección de `m()` `0x08048468` contenga la dirección de `n()` `0x08048454`.
4. **Explotación:** Al llegar a al instruciión `call eax` el programa saltará a la función `n()` la cual tiene privilegios de `level7` y ejecutará el comando para leer `.pass`