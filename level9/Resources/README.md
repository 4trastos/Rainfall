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
6. <+10>: En `[ebp+0x8]` es donde está `int argc` y compara el número de argumentos con 1.
7. <+14>: `*JG*` (Jump if Greate) Si `argc` es mayor que 1 ha recibido argumentos y salta a la línea <+28>
8. <+16>: Pone un 1 en el stack como código de salida 
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

### **Líneas 62, 69, 74, 76, 84, 87 y 92:**
```asm
0x08048632 <+62>:	mov    DWORD PTR [esp],0x6c
0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>
0x0804863e <+74>:	mov    ebx,eax
0x08048640 <+76>:	mov    DWORD PTR [esp+0x4],0x6
0x08048648 <+84>:	mov    DWORD PTR [esp],ebx
0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>
0x08048650 <+92>:	mov    DWORD PTR [esp+0x18],ebx
```

1. <+62>: Pone 108 (0x6c) en el stack. Tamaño en bytes.
2. <+69>: Hace de nuevo un `new` reservando 108 bytes.
3. <+74>: Guarda la dirección del nuevo objeto en EBX
4. <+76>: Pone un 6 (0x6) coo segundo argumento: `[esp+0x4]`
5. <+84>: Pone la dirección del segundo obejto como primer argumento: `[esp]`
6. <+87>: Llama a `N::N(int)` (`0x80486f6`). Constructor de la clase `N`.
7. <+92>: Guarda el puentero del nuevo objeto en `[esp+0x18]`.

### **Líneas 96, 100, 104, 108 y 112:**
```asm
0x08048654 <+96>:	mov    eax,DWORD PTR [esp+0x1c]
0x08048658 <+100>:	mov    DWORD PTR [esp+0x14],eax
0x0804865c <+104>:	mov    eax,DWORD PTR [esp+0x18]
0x08048660 <+108>:	mov    DWORD PTR [esp+0x10],eax
0x08048664 <+112>:	mov    eax,DWORD PTR [ebp+0xc]
```

1. <+96>: Recupera en EAX el puntero del primer objeto: `[esp+0x1c]`
2. <+100>: Guarda el puntero del primer objeto en `[esp+0x14]`
3. <+104>: Recupera en EAX el puntero del segundo objeto: `[esp+0x18]`
4. <+108>: Guarda el puntero del segundo objeto en `[esp+0x10]`
5. <+112>: Carga en EAX: `char **argv` 

### **Líneas 115, 118, 120, 124, 128 y 131:**
```asm
0x08048667 <+115>:	add    eax,0x4
0x0804866a <+118>:	mov    eax,DWORD PTR [eax]
0x0804866c <+120>:	mov    DWORD PTR [esp+0x4],eax
0x08048670 <+124>:	mov    eax,DWORD PTR [esp+0x14]
0x08048674 <+128>:	mov    DWORD PTR [esp],eax
0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
```
1. <+115>: Avanza 4 bytes para apuntar a `argv[1]`
2. <+118>: Actualiza EAX con el valor de `argv[1]`.
3. <+120>: Guarda `argv[1]` en el stack `[esp+0x4]` como segundo argumento
4. <+124>: Carga en EAX el puntero del primer objeto `[esp+0x14]`
5. <+128>: Y lo pone en el stack como primer argumento.
6. <+131>: Llama a la función: `N::setAnnotation(argv[1])`

### **Analizamos la función N::setAnnotation(char *) 0x804870e**

```bash
(gdb) disas 0x0804870e
Dump of assembler code for function _ZN1N13setAnnotationEPc:
   0x0804870e <+0>:	    push   ebp
   0x0804870f <+1>:	    mov    ebp,esp
   0x08048711 <+3>:	    sub    esp,0x18
   0x08048714 <+6>:	    mov    eax,DWORD PTR [ebp+0xc]
   0x08048717 <+9>:	    mov    DWORD PTR [esp],eax
   0x0804871a <+12>:	call   0x8048520 <strlen@plt>
   0x0804871f <+17>:	mov    edx,DWORD PTR [ebp+0x8]
   0x08048722 <+20>:	add    edx,0x4
   0x08048725 <+23>:	mov    DWORD PTR [esp+0x8],eax
   0x08048729 <+27>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804872c <+30>:	mov    DWORD PTR [esp+0x4],eax
   0x08048730 <+34>:	mov    DWORD PTR [esp],edx
   0x08048733 <+37>:	call   0x8048510 <memcpy@plt>
   0x08048738 <+42>:	leave  
   0x08048739 <+43>:	ret    
End of assembler dump.
```

1. <+0>: Guarda en el stack el valor de EBP (main).
2. <+1>: Copia el EBP el nuevo ESP
3. <+3>: Reserva 24 bytes (0x18) para que la función organice variables locales y arguemntos. 
4. <+6>: Carga en EAX el argumento recibido (`argv[1]`).
5. <+9>: Guarda `argv[1]` en el stack como arguento para la función `strlen()`
6. <+12>: Llama a `strlen(argv[1])` (el resultado se queda en EAX)
7. <+17> y <+20>: Carga la dirección del objeto actual `(this)` y le suma 4 bytes. (los primeros 4 bytes del objeto están reservados para el puntero a la `vtable`).
8. <+23> a <+34>: Prepara los argumentos para `memcpy(destino, origen, tamaño)`.
- destino: `Object_01` + 4
- origen: `argv[1]`
- tamaño: `strlen(argv[1])`
9. <+37>: Llama a `memcpy()` => **IMPORTANTE => Vulnerabilidad encontrada.**
- `memcpy` no tiene límite. Si `argv[1]` mide más de `104 bytes` (108 del objeto - 4 del vptr), podemos desbordar el `Object_01`
- Como el `Object_02` se reservó justo después en el Heap, podemos escribir encima de `Object_02`.

### **Líneas 136, 140, 142, 144, 148, 152, 156 y 159:**
```asm
0x0804867c <+136>:	mov    eax,DWORD PTR [esp+0x10]
0x08048680 <+140>:	mov    eax,DWORD PTR [eax]
0x08048682 <+142>:	mov    edx,DWORD PTR [eax]
0x08048684 <+144>:	mov    eax,DWORD PTR [esp+0x14]
0x08048688 <+148>:	mov    DWORD PTR [esp+0x4],eax
0x0804868c <+152>:	mov    eax,DWORD PTR [esp+0x10]
0x08048690 <+156>:	mov    DWORD PTR [esp],eax
0x08048693 <+159>:	call   edx
```

1. <+136> a <+142>: El programa va a buscar una función dentro del segundo objeto (`Object_02`). Primero carga la dirección del objeto, luego mira sus primeros 4 bytes (donde está la `vtable`) y por último saca la dirección de la función.
2. <+159>: Llama a esa función (`call edx`). Si hemos hackeado la memoria, aquí es donde tomamos el control.

### **Líneas 161, 164 y 165: (Salida del programa)**
```asm
0x08048695 <+161>:	mov    ebx,DWORD PTR [ebp-0x4]
0x08048698 <+164>:	leave  
0x08048699 <+165>:	ret   
```

### **Resumen del Flujo de Ataque para el Nivel 9**

1. **Lectura:** El programa recibe un argumento por línea de comandos (`argv[1]`). Al ser C++, utiliza `operator new` para reservar espacio para dos objetos de la Clase `N` en el **Heap**. Cada objeto ocupa **108 bytes** (0x6c).

* **Object_01** (`esp+0x1c`): Se inicializa con el valor 5.
* **Object_02** (`esp+0x18`): Se inicializa con el valor 6.

2. **Vulnerabilidad:** Existe un **Heap-based Buffer Overflow** en la función `N::setAnnotation`.

* En la línea `<+131>`, el programa llama a `setAnnotation(argv[1])` para el primer objeto.
* Dentro de esa función (línea `<+37>`), se ejecuta un `memcpy(destino, origen, tamaño)`.
* El **destino** es el buffer del Objeto 1 (empezando en el byte 4).
* El **tamaño** lo dicta el `strlen` de nuestra entrada. Como no hay control de límites, si enviamos más de 104 bytes, desbordamos el `Object_01` y empezamos a escribir encima del **Object_02**.

3. **Objetivo:** Sobrescribir el puntero de la **Vtable** del `Object_02` y redirigirlo a nuestro Shellcode.

* En C++, los primeros 4 bytes de un objeto con métodos virtuales contienen la dirección de la **Vtable** (una tabla de punteros a funciones).
* En la línea `<+159>`, el programa hace un `call edx`. El valor de `edx` se extrae directamente de lo que hay en los primeros 4 bytes del `Object_02` (líneas `<+140>` y `<+142>`).
* Si logramos que esos primeros 4 bytes apunten a una dirección controlada por nosotros, el `call edx` ejecutará nuestro código.

4. **Estrategia (El Layout del Heap):**

* Debemos llenar el buffer del primer objeto con nuestro **Shellcode**.
* Necesitamos calcular el *padding* exacto (basura) para llegar hasta el inicio del `Object_02`. Dado que cada objeto mide 108 bytes, la distancia entre el inicio de los datos del `Object_01` y el inicio del `Object_02` es de **104 bytes**.
* **El Truco**: No podemos apuntar `edx` directamente al Shellcode porque el código hace un "doble salto": busca en la dirección que le damos (`eax`) y salta a lo que haya escrito en esa dirección (`[eax]`). Por tanto, necesitamos que el puntero a la Vtable apunte a una dirección que a su vez contenga la dirección de nuestro Shellcode.

5. **Explotación:**

* **Paso 1:** Inyectamos el Shellcode al principio de nuestro buffer (en el `Object_01`).
* **Paso 2:** Rellenamos con basura hasta llegar al byte 104.
* **Paso 3:** En los bytes 104-108 (que caen justo en el inicio del `Object_02`), escribimos la dirección de memoria donde hemos guardado nuestro Shellcode (normalmente una dirección del Heap que podemos ver con `gdb`).
* **Paso 4:** El programa ejecuta `main` y es donde realiza la llamada al Shellcode:

```asm
0x08048693 <+159>:  call   edx   ; EDX ahora contiene la dirección de nuestro Shellcode
```

* **Resultado:** El programa intenta llamar a un método virtual del `Object_02`, pero como hemos "hackeado" su puntero de Vtable, acaba saltando a nuestro Shellcode y ejecutando `/bin/sh` con privilegios de `bonus0`.
