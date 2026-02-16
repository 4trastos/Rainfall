# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level7**

```asm
Dump of assembler code for function main:
   0x08048521 <+0>:	    push   ebp
   0x08048522 <+1>:	    mov    ebp,esp
   0x08048524 <+3>:	    and    esp,0xfffffff0
   0x08048527 <+6>:	    sub    esp,0x20
   0x0804852a <+9>:     mov    DWORD PTR [esp],0x8
   0x08048531 <+16>:	   call   0x80483f0 <malloc@plt>
   0x08048536 <+21>:	   mov    DWORD PTR [esp+0x1c],eax
   0x0804853a <+25>:	   mov    eax,DWORD PTR [esp+0x1c]
   0x0804853e <+29>:	   mov    DWORD PTR [eax],0x1
   0x08048544 <+35>:	   mov    DWORD PTR [esp],0x8
   0x0804854b <+42>:	   call   0x80483f0 <malloc@plt>
   0x08048550 <+47>:	   mov    edx,eax
   0x08048552 <+49>:	   mov    eax,DWORD PTR [esp+0x1c]
   0x08048556 <+53>:	   mov    DWORD PTR [eax+0x4],edx
   0x08048559 <+56>:	   mov    DWORD PTR [esp],0x8
   0x08048560 <+63>:	   call   0x80483f0 <malloc@plt>
   0x08048565 <+68>:	   mov    DWORD PTR [esp+0x18],eax
   0x08048569 <+72>:	   mov    eax,DWORD PTR [esp+0x18]
   0x0804856d <+76>:	   mov    DWORD PTR [eax],0x2
   0x08048573 <+82>:	   mov    DWORD PTR [esp],0x8
   0x0804857a <+89>:	   call   0x80483f0 <malloc@plt>
   0x0804857f <+94>:	   mov    edx,eax
   0x08048581 <+96>:	   mov    eax,DWORD PTR [esp+0x18]
   0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx
   0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804858b <+106>:	add    eax,0x4
   0x0804858e <+109>:	mov    eax,DWORD PTR [eax]
   0x08048590 <+111>:	mov    edx,eax
   0x08048592 <+113>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048596 <+117>:	mov    eax,DWORD PTR [eax+0x4]
   0x08048599 <+120>:	mov    DWORD PTR [esp+0x4],edx
   0x0804859d <+124>:	mov    DWORD PTR [esp],eax
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
   0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]
   0x080485a8 <+135>:	add    eax,0x8
   0x080485ab <+138>:	mov    eax,DWORD PTR [eax]
   0x080485ad <+140>:	mov    edx,eax
   0x080485af <+142>:	mov    eax,DWORD PTR [esp+0x18]
   0x080485b3 <+146>:	mov    eax,DWORD PTR [eax+0x4]
   0x080485b6 <+149>:	mov    DWORD PTR [esp+0x4],edx
   0x080485ba <+153>:	mov    DWORD PTR [esp],eax
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
   0x080485c2 <+161>:	mov    edx,0x80486e9
   0x080485c7 <+166>:	mov    eax,0x80486eb
   0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx
   0x080485d0 <+175>:	mov    DWORD PTR [esp],eax
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
   0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax
   0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44
   0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
   0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
   0x080485fc <+219>:	mov    eax,0x0
   0x08048601 <+224>:	leave  
   0x08048602 <+225>:	ret    
End of assembler dump.
```

### **Líneas 0, 1, 3 y 6:**
```asm
0x08048521 <+0>:	    push   ebp
0x08048522 <+1>:	    mov    ebp,esp
0x08048524 <+3>:	    and    esp,0xfffffff0
0x08048527 <+6>:	    sub    esp,0x20
```

1. Guardan en lo alto del stack el valor de EBP (CPU): [ebo + 0x00]. Guarda el suelo para no perderlo y ESP se mueve 4 bytes hacia abajo.
2. Copia en EBP el nuevo ESP del stack para la función `main`
3. Alinea el stack como múltiplo de 16 bytes. (los 4 último byest de ESP se ponen a 0)
4. Reserva (desplaza) 32 bytes (0x20). Espacio que el compilador reserva en el stack de `main` para organizar las variables locales y los argumentos de las funciones que `main` va a llamar.

### **Líneas 9, 16, 21, 25, 29, 35, 42 y 47:**
```asm
0x0804852a <+9>:     mov    DWORD PTR [esp],0x8
0x08048531 <+16>:	   call   0x80483f0 <malloc@plt>       ⟸ 1º malloc
0x08048536 <+21>:	   mov    DWORD PTR [esp+0x1c],eax
0x0804853a <+25>:	   mov    eax,DWORD PTR [esp+0x1c]
0x0804853e <+29>:	   mov    DWORD PTR [eax],0x1
0x08048544 <+35>:	   mov    DWORD PTR [esp],0x8
0x0804854b <+42>:	   call   0x80483f0 <malloc@plt>       ⟸ 2º malloc
0x08048550 <+47>:	   mov    edx,eax
```

1. <+9>: Pone 8 bytes (0x8) en lo alto del stack como argumeto para `malloc`
2. <+16>: Llama a la función `malloc` y reserva los 8 bytes para ls `struct_01`.
3. <+21>: Guarda la dirección del primer bloque (EAX) en **[esp+0x1c]** . Guardado en el `heap`.
4. <+25>: Actualiza EAX con la dirección del `struct_01`. 
5. <+29>: Escribe el nº 1 (un entero de 4 bytes) en el inicio de la memoria (eax). Es una asignación de valor: `struct_01->id = 1`.
6. <+35>: Pone 8 bytes (0x08) en ESP como arguento para el 2º `malloc`
7. <+42>: Llama a la función `malloc()` y reserva los 8 bytes para `buffer_01` (donde irá el texto). 
8. <+47>: Copia en EDX  la direcció de `buffer_01` (EAX).

### **Líneas 49, 53, 56, 63, 68, 72, 82, 89 y 94:**
```asm
0x08048552 <+49>:	   mov    eax,DWORD PTR [esp+0x1c]
0x08048556 <+53>:	   mov    DWORD PTR [eax+0x4],edx
0x08048559 <+56>:	   mov    DWORD PTR [esp],0x8
0x08048560 <+63>:	   call   0x80483f0 <malloc@plt>       ⟸ 3º malloc
0x08048565 <+68>:	   mov    DWORD PTR [esp+0x18],eax
0x08048569 <+72>:	   mov    eax,DWORD PTR [esp+0x18]
0x0804856d <+76>:	   mov    DWORD PTR [eax],0x2
0x08048573 <+82>:	   mov    DWORD PTR [esp],0x8
0x0804857a <+89>:	   call   0x80483f0 <malloc@plt>       ⟸ 4º malloc
0x0804857f <+94>:	   mov    edx,eax
```

1.  <+49>: Recupera la dirección de `struct_01` del heap **[esp+0x1c]** en el registro EAX.
2.  <+53>: Guarda en EAX [eax+0x4] (los segundos 4 bytes del struct) la dirección de `buffer_01` que estaba en EDX. **Aquí se enlaza el struct con su buffer de datos**.
3.  <+56>: Pone 8 bytes (0x08) en ESP (stack) para usarlo como argumento del 3º `malloc`
4.  <+63>: Llama a la función `malloc` y reserva los 8 bytes para `struct_02`
5.  <+68>: Guarda la dirección del segundo bloque (`struct_02`) en  **[esp+0x18]** del stack.
6.  <+72>: Recupera EAX con la dirección de `struct_02`
7.  <+76>: Asigna el valor 2 al inicio del struct: `struct_02->id = 2`. 
8.  <+82>: Pone 8 bytes (0x08) en ESP para usarlos como argumento para el 4º `malloc`
9.  <+89>: Llama a la función `malloc` y reserva 8 bytes (0x08) para el `buffer_02`.
10. <+94>: Copia en EDX la dirección de EAX (`buffer_02`).

### **Líneas 96, 100, 103, 106, 109, 111, 113, 117, 120, 124 y 127:**
```asm
0x08048581 <+96>:	   mov    eax,DWORD PTR [esp+0x18]
0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx
0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]
0x0804858b <+106>:	add    eax,0x4
0x0804858e <+109>:	mov    eax,DWORD PTR [eax]
0x08048590 <+111>:	mov    edx,eax
0x08048592 <+113>:	mov    eax,DWORD PTR [esp+0x1c]
0x08048596 <+117>:	mov    eax,DWORD PTR [eax+0x4]
0x08048599 <+120>:	mov    DWORD PTR [esp+0x4],edx
0x0804859d <+124>:	mov    DWORD PTR [esp],eax
0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
```

1.  <+96>:  Recupera en EAX la dirección de la `struct_02`.
2.  <+100>: Guarda en `[eax+0x4]` la dirección del `buffer_02`.
3.  <+103>: Carga en EAX `char **argv` ([ebp+0xc]).
4.  <+106>: Suma 4 bytes (0x04) para apuntar a `argv[1]`
5.  <+109>: Recupera EAX con la dirección del string de argv[1].
6.  <+111>: Copia esa dirección string (argv[1]) en EDX. Es el `src` de `strcpy()`.
7.  <+113>: Recupera en EAX la dirección del `struct_01` [esp+0x1c].
8.  <+117>: Mueve a EAX el contenido de [eax+0x4]. **IMPORTANET**: EAX ahora contiene la dirección de `buffer_01` (dst).
9.  <+120>: Desplaza 4 bytes el stack y pone en [esp+0x4] el `src` (edx = `argv[1]`).
10. <+124>: Pone en lo alto del stack el `dst` (eax = `buffer_01`).
11. <+127>: Llama a `strcpy(buffer_01, argv[1])`.

### **Líneas 132, 135, 138, 140, 142, 146, 149, 153, 156, 161 y 166: (segundo strcpy)**
```asm
0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]      ; Carga argv
0x080485a8 <+135>:	add    eax,0x8                      ; Suma 8 bytes para apuntar a argv[2]
0x080485ab <+138>:	mov    eax,DWORD PTR [eax]          ; Actualiza eax con la dirección de argv[2]
0x080485ad <+140>:	mov    edx,eax                      ; Guarda en edx la direccion de argv[2]
0x080485af <+142>:	mov    eax,DWORD PTR [esp+0x18]
0x080485b3 <+146>:	mov    eax,DWORD PTR [eax+0x4]
0x080485b6 <+149>:	mov    DWORD PTR [esp+0x4],edx
0x080485ba <+153>:	mov    DWORD PTR [esp],eax
0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
0x080485c2 <+161>:	mov    edx,0x80486e9
0x080485c7 <+166>:	mov    eax,0x80486eb
```

1. <+132> al <+140>: Hace los mismo para obtener `argv[2]`.
2. <+142>: Recupera en EAX la dirección de la `struct_02`.
3. <+146>: Mueve a EAX el contentenido de [eax+0x4]: **IMPORTANTE**: EAX ahora contine la dirección de `buffer_02` (dst)
4. <+149>: Desplaza 4 bytes el stack y pone en [esp+0x4] la dirección de `argv[2]` (src).
5. <+153>: Pone en lo alto del stack el `dst` que es `buffer_02` (eax)
6. <+156>: Llama a `strcpy(buffer_02, argv[2])`
**(Apertura del archivo):**
7. <+161>: Carga en EDX la dirección de memoria: `0x80486e9` (string "r").
8. <+166>: Carga en EAX el valor de la dirección de memoria: `0x80486eb` (dirección de la ruta).
- Vemos con `gdb` que en la dirección `0x80486e9` imprime el string "r":
```bash
(gdb) x/s 0x80486e9
0x80486e9:	 "r"
```
- Y que en la dirrección `0x80486eb` se encuentra la ruta del password del level8:
```bash
(gdb) x/s 0x80486eb
0x80486eb:	 "/home/user/level8/.pass"
```

### **Líneas 171, 175 y 178:**
```asm
0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx
0x080485d0 <+175>:	mov    DWORD PTR [esp],eax
0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
```

1. <+171>: Desplaza 4 bytes el stack y pone en [esp+0x4] el contenido de `0x80486e9` ("r"). Segundo argumento de `fopen()`. 
2. <+175>: Guarda en lo alto del stack la ruta: `"/home/user/level8/.pass"`. El primer argumento de `fopen()`
3. Llama a a`fopen("/home/user/level8/.pass", "r")`.


### **Líneas 183, 187, 195 y 202: (Lectura del password)**
```asm
0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax
0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44
0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960
0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
```

1. <+183>: Desplaza 8 bytes el stack y almacena en [esp+0x8] el contenido de EAX (El puntero al archivo devuelto por `fopen`) `FILE_pointer`.
2. <+187>:Desplaza 4 bytes el stack y Pone el tamaño 68 (0x44) en [esp+0x4] -> segundo argumento de `fgets()`.
3. <+195>: Copia en lo alto del stack la dirección `0x8049960` que contiene un string vacio `""`, que es la dirección de destino en `fgets()`:
```bash
(gdb) x/s 0x8049960
0x8049960 <c>:	 ""
```
4. <+202> Llama a `fgets(0x8049960, 68, FILE_pointer)`. **IMPORTANTE**: El password se guarda en la memoria global 0x8049960.

### **Líneas 207, 214 y 219:**
```asm
0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703
0x080485f7 <+214>:	call   0x8048400 <puts@plt>
0x080485fc <+219>:	mov    eax,0x0
```

1. <+207>: Mueve a los alto del stack el contenido de la dirección: `0x8048703`:
```bash
(gdb) x/s 0x8048703
0x8048703:	 "~~"
``` 
2. <+214>: Llama a `puts()` que muestra en el terminal: "~~"
3. <+219>: Borra el contenido de EAX

### **Línea 224: `leave`**
```asm
0x08048601 <+224>:	leave  
```
- **Equivale a:**
```asm
mov   esp,ebp  ; Restaurar ESP
pop   ebp      ; Restaurar EBP antiguo
```

### **Línea 225: `ret`**
```asm
0x08048602 <+225>:	ret
```
- **Acción**: Return - Saca la dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución del `main()`.

### **Resumen del Flujo de Ataque para el Nivel 7**

1. **Lectura:** El programa reserva cuatro bloques en el `heap`. Crea dos estructuras `(struct_01 y struct_02)`, y cada una tiene un puntero que apunta a su propio `buffer` de datos `(buffer_01 y buffer_02)`. Al final, el programa lee el `password` del siguiente nivel y la guarda en una variable global en la dirección `0x08049960`, pero solo imprime `"~~"` mediante la función `puts()`.
2. **Vulnerabilidad:** El programa utiliza `strcpy` dos veces. La primera copia `argv[1]` en `buffer_01` y la segunda copia `argv[2]` en `buffer_02`. Como no hay control de tamaño, podemos causar un **Heap-based Buffer Overflow**. Con el primer `strcpy (argv[1])`, podemos desbordar el `buffer_01` hasta llegar a la `struct_02` y sobrescribir el puntero que indica dónde debe escribir el segundo `strcpy`.
3. **Objetivo:** Engañar al segundo `strcpy` para que escriba en un lugar determinado de la memoria. En lugar de escribir en el `buffer_02`, queremos que escriba en la entrada de la **GOT (Global Offset Table)** de la función `puts()`. 
4. **¿Qué dirección ponemos?**: 
- Buscamos cual es la función de llama a `printf()` y printea la dirección `0x08049960` que es la que almacena el password:
```bash
(gdb) info functions 
0x080484f4  m                                               ⟸
0x08048521  main
(gdb) disas m
Dump of assembler code for function m:
   0x080484f4 <+0>:	push   ebp
   0x080484f5 <+1>:	mov    ebp,esp
   0x080484f7 <+3>:	sub    esp,0x18
   0x080484fa <+6>:	mov    DWORD PTR [esp],0x0
   0x08048501 <+13>:	call   0x80483d0 <time@plt>
   0x08048506 <+18>:	mov    edx,0x80486e0
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960   ⟸
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>          ⟸
   0x0804851f <+43>:	leave  
   0x08048520 <+44>:	ret    
End of assembler dump.

```

- Nuestro objetivo es que el segundo `strcpy (argv[2])` guarde ahí la dirección de la función `m() (0x080484f4)`, que es la que sí imprime el password.
4. **Explotación:**
* **Paso 1:** Usamos `argv[1]` con el tamaño justo de "relleno" para alcanzar el puntero de la `struct_02` y lo sobrescribimos con la dirección de `puts` en la GOT.
* **Paso 2:** Usamos `argv[2]` para enviarle la dirección de la función `m()`.
* **Resultado:** Cuando el código llegue a la línea `<+214>`, el programa creerá que está llamando a `puts()`, pero como hemos "hackeao" su dirección en la tabla GOT, saltará a `m()` y nos mostrará el password.


