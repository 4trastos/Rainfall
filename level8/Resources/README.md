# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level8**

```asm
Dump of assembler code for function main:
   0x08048564 <+0>:	    push   ebp
   0x08048565 <+1>:	    mov    ebp,esp
   0x08048567 <+3>:	    push   edi
   0x08048568 <+4>:	    push   esi
   0x08048569 <+5>:	    and    esp,0xfffffff0
   0x0804856c <+8>:	    sub    esp,0xa0
   0x08048572 <+14>:	jmp    0x8048575 <main+17>
   0x08048574 <+16>:	nop
   0x08048575 <+17>:	mov    ecx,DWORD PTR ds:0x8049ab0
   0x0804857b <+23>:	mov    edx,DWORD PTR ds:0x8049aac
   0x08048581 <+29>:	mov    eax,0x8048810
   0x08048586 <+34>:	mov    DWORD PTR [esp+0x8],ecx
   0x0804858a <+38>:	mov    DWORD PTR [esp+0x4],edx
   0x0804858e <+42>:	mov    DWORD PTR [esp],eax
   0x08048591 <+45>:	call   0x8048410 <printf@plt>
   0x08048596 <+50>:	mov    eax,ds:0x8049a80
   0x0804859b <+55>:	mov    DWORD PTR [esp+0x8],eax
   0x0804859f <+59>:	mov    DWORD PTR [esp+0x4],0x80
   0x080485a7 <+67>:	lea    eax,[esp+0x20]
   0x080485ab <+71>:	mov    DWORD PTR [esp],eax
   0x080485ae <+74>:	call   0x8048440 <fgets@plt>
   0x080485b3 <+79>:	test   eax,eax
   0x080485b5 <+81>:	je     0x804872c <main+456>
   0x080485bb <+87>:	lea    eax,[esp+0x20]
   0x080485bf <+91>:	mov    edx,eax
   0x080485c1 <+93>:	mov    eax,0x8048819
   0x080485c6 <+98>:	mov    ecx,0x5
   0x080485cb <+103>:	mov    esi,edx
   0x080485cd <+105>:	mov    edi,eax
   0x080485cf <+107>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080485d1 <+109>:	seta   dl
   0x080485d4 <+112>:	setb   al
   0x080485d7 <+115>:	mov    ecx,edx
   0x080485d9 <+117>:	sub    cl,al
   0x080485db <+119>:	mov    eax,ecx
   0x080485dd <+121>:	movsx  eax,al
   0x080485e0 <+124>:	test   eax,eax
   0x080485e2 <+126>:	jne    0x8048642 <main+222>
   0x080485e4 <+128>:	mov    DWORD PTR [esp],0x4
   0x080485eb <+135>:	call   0x8048470 <malloc@plt>
   0x080485f0 <+140>:	mov    ds:0x8049aac,eax
   0x080485f5 <+145>:	mov    eax,ds:0x8049aac
   0x080485fa <+150>:	mov    DWORD PTR [eax],0x0
   0x08048600 <+156>:	lea    eax,[esp+0x20]
   0x08048604 <+160>:	add    eax,0x5
   0x08048607 <+163>:	mov    DWORD PTR [esp+0x1c],0xffffffff
   0x0804860f <+171>:	mov    edx,eax
   0x08048611 <+173>:	mov    eax,0x0
   0x08048616 <+178>:	mov    ecx,DWORD PTR [esp+0x1c]
   0x0804861a <+182>:	mov    edi,edx
   0x0804861c <+184>:	repnz scas al,BYTE PTR es:[edi]
   0x0804861e <+186>:	mov    eax,ecx
   0x08048620 <+188>:	not    eax
   0x08048622 <+190>:	sub    eax,0x1
   0x08048625 <+193>:	cmp    eax,0x1e
   0x08048628 <+196>:	ja     0x8048642 <main+222>
   0x0804862a <+198>:	lea    eax,[esp+0x20]
   0x0804862e <+202>:	lea    edx,[eax+0x5]
   0x08048631 <+205>:	mov    eax,ds:0x8049aac
   0x08048636 <+210>:	mov    DWORD PTR [esp+0x4],edx
   0x0804863a <+214>:	mov    DWORD PTR [esp],eax
   0x0804863d <+217>:	call   0x8048460 <strcpy@plt>
   0x08048642 <+222>:	lea    eax,[esp+0x20]
   0x08048646 <+226>:	mov    edx,eax
   0x08048648 <+228>:	mov    eax,0x804881f
   0x0804864d <+233>:	mov    ecx,0x5
   0x08048652 <+238>:	mov    esi,edx
   0x08048654 <+240>:	mov    edi,eax
   0x08048656 <+242>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x08048658 <+244>:	seta   dl
   0x0804865b <+247>:	setb   al
   0x0804865e <+250>:	mov    ecx,edx
   0x08048660 <+252>:	sub    cl,al
   0x08048662 <+254>:	mov    eax,ecx
   0x08048664 <+256>:	movsx  eax,al
   0x08048667 <+259>:	test   eax,eax
   0x08048669 <+261>:	jne    0x8048678 <main+276>
   0x0804866b <+263>:	mov    eax,ds:0x8049aac
   0x08048670 <+268>:	mov    DWORD PTR [esp],eax
   0x08048673 <+271>:	call   0x8048420 <free@plt>
   0x08048678 <+276>:	lea    eax,[esp+0x20]
   0x0804867c <+280>:	mov    edx,eax
   0x0804867e <+282>:	mov    eax,0x8048825
   0x08048683 <+287>:	mov    ecx,0x6
   0x08048688 <+292>:	mov    esi,edx
   0x0804868a <+294>:	mov    edi,eax
   0x0804868c <+296>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x0804868e <+298>:	seta   dl
   0x08048691 <+301>:	setb   al
   0x08048694 <+304>:	mov    ecx,edx
   0x08048696 <+306>:	sub    cl,al
   0x08048698 <+308>:	mov    eax,ecx
   0x0804869a <+310>:	movsx  eax,al
   0x0804869d <+313>:	test   eax,eax
   0x0804869f <+315>:	jne    0x80486b5 <main+337>
   0x080486a1 <+317>:	lea    eax,[esp+0x20]
   0x080486a5 <+321>:	add    eax,0x7
   0x080486a8 <+324>:	mov    DWORD PTR [esp],eax
   0x080486ab <+327>:	call   0x8048430 <strdup@plt>
   0x080486b0 <+332>:	mov    ds:0x8049ab0,eax
   0x080486b5 <+337>:	lea    eax,[esp+0x20]
   0x080486b9 <+341>:	mov    edx,eax
   0x080486bb <+343>:	mov    eax,0x804882d
   0x080486c0 <+348>:	mov    ecx,0x5
   0x080486c5 <+353>:	mov    esi,edx
   0x080486c7 <+355>:	mov    edi,eax
   0x080486c9 <+357>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080486cb <+359>:	seta   dl
   0x080486ce <+362>:	setb   al
   0x080486d1 <+365>:	mov    ecx,edx
   0x080486d3 <+367>:	sub    cl,al
   0x080486d5 <+369>:	mov    eax,ecx
   0x080486d7 <+371>:	movsx  eax,al
   0x080486da <+374>:	test   eax,eax
   0x080486dc <+376>:	jne    0x8048574 <main+16>
   0x080486e2 <+382>:	mov    eax,ds:0x8049aac
   0x080486e7 <+387>:	mov    eax,DWORD PTR [eax+0x20]
   0x080486ea <+390>:	test   eax,eax
   0x080486ec <+392>:	je     0x80486ff <main+411>
   0x080486ee <+394>:	mov    DWORD PTR [esp],0x8048833
   0x080486f5 <+401>:	call   0x8048480 <system@plt>
   0x080486fa <+406>:	jmp    0x8048574 <main+16>
   0x080486ff <+411>:	mov    eax,ds:0x8049aa0
   0x08048704 <+416>:	mov    edx,eax
   0x08048706 <+418>:	mov    eax,0x804883b
   0x0804870b <+423>:	mov    DWORD PTR [esp+0xc],edx
   0x0804870f <+427>:	mov    DWORD PTR [esp+0x8],0xa
   0x08048717 <+435>:	mov    DWORD PTR [esp+0x4],0x1
   0x0804871f <+443>:	mov    DWORD PTR [esp],eax
   0x08048722 <+446>:	call   0x8048450 <fwrite@plt>
   0x08048727 <+451>:	jmp    0x8048574 <main+16>
   0x0804872c <+456>:	nop
   0x0804872d <+457>:	mov    eax,0x0
   0x08048732 <+462>:	lea    esp,[ebp-0x8]
   0x08048735 <+465>:	pop    esi
   0x08048736 <+466>:	pop    edi
   0x08048737 <+467>:	pop    ebp
   0x08048738 <+468>:	ret    
End of assembler dump.
```

### **Líneas 0, 1, 2, 3, 4, 5, 8 y 14:**
```asm
0x08048564 <+0>:	push   ebp
0x08048565 <+1>:	mov    ebp,esp
0x08048567 <+3>:	push   edi
0x08048568 <+4>:	push   esi
0x08048569 <+5>:	and    esp,0xfffffff0
0x0804856c <+8>:    sub    esp,0xa0
0x08048572 <+14>:	jmp    0x8048575 <main+17>
```

1. <+0>: Guarda en lo alto del stack el valor de EBP (CPU):  [ebp + 0x00] y ESP se desplaza 4 bytes hacia abajo.
2. <+1>: Copia en EBP el nuevo ESP del stack para la función `main()`
3. <+3>: Guarda en el stack el registro preservado EDI para usarlo más tarde y ESP se desplaza 4 bytes hacia abajo
4. <+4>: Guarda en el stack el registro preservado ESI para usarlo mas tarde y ESP se desplaza 4 bytes hacia abajo
5. <+5>: Alinea el stack como múltiplo de 16 bytes. (los 4 últimos bytes de ESP se ponen a 0).
6. <+8>: Reserva (desplaza) 160 bytes (0xa0). Espacio para el compilador en el stack de `main()` para organizar las variables locales y los argumentos de las funciones que `main()` va a llamar.
7. <+14>: Salta a la línea <+17> dentro de la función `main()`

### **Líneas 16, 17, 23, 29, 34, 38, 42 y 45:**
```asm
0x08048574 <+16>:	nop
0x08048575 <+17>:	mov    ecx,DWORD PTR ds:0x8049ab0
0x0804857b <+23>:	mov    edx,DWORD PTR ds:0x8049aac
0x08048581 <+29>:	mov    eax,0x8048810
0x08048586 <+34>:	mov    DWORD PTR [esp+0x8],ecx
0x0804858a <+38>:	mov    DWORD PTR [esp+0x4],edx
0x0804858e <+42>:	mov    DWORD PTR [esp],eax
0x08048591 <+45>:	call   0x8048410 <printf@plt>
```

1. <+16>: NO operativo (no hace nada).
2. <+17>: Copia en ECX la dirección `0x8049ab0` de la variable global `service`:
```bash
(gdb) x/s 0x8049ab0
0x8049ab0 <service>:	 ""   ⟸ (puntero de 4 bytes en la sección .data / .bss)
```
3. <+23>: Copia en EDX la dirección `0x8049aac` de la variable global `auth`:
```bash
(gdb) x/s 0x8049aac
0x8049aac <auth>:	 ""       ⟸ (puntero de 4 bytes en la sección .data / .bss)
```
4. <+29>: Copia en EAX el contenido de la dirección `0x8048810` que contiene dos direcciones de memoria:
```bash
(gdb) x/s 0x8048810
0x8048810:	 "%p, %p \n"
```
5. <+34>: Desplaza 8 bytes el stack [esp+0x8] y guarda ECX (`service`).
6. <+38>: Desplaza 4 bytes más el stack [esp+0x4] y guarda EDX (`auth`).
7. <+42>: Copia el lo alto del stack EAX: `0x8048810` -> `"%p, %p \n"`
8. <+45>: Llama a `printf()` y muestra las direcciones de memora de `service` y `auth`:
- `printf("%p, %p \n", service, auth)`

### **Líneas 50, 55, 59, 67, 71 y 74:**
```asm
0x08048596 <+50>:	mov    eax,ds:0x8049a80
0x0804859b <+55>:	mov    DWORD PTR [esp+0x8],eax
0x0804859f <+59>:	mov    DWORD PTR [esp+0x4],0x80
0x080485a7 <+67>:	lea    eax,[esp+0x20]
0x080485ab <+71>:	mov    DWORD PTR [esp],eax
0x080485ae <+74>:	call   0x8048440 <fgets@plt>
```

1. <+50>: Carga en EAX la dirección de la varibel global (buffer) de la función `stdin()`
```bash
(gdb) x/s 0x8049a80
0x8049a80 <stdin@@GLIBC_2.0>:	 ""
```
2. <+55>: Desplaza 8 byets el stack y guarda en `[esp+0x8]` la dirección del  `buffer` de `stdin()`.
3. <+59>: Desplaza 4 bytes el stack y pone en `[esp+0x4]` el 128 (0x80). `int size` (argumento de fgets() -> el tamaño máximo que leerá).
4. <+67>: Calcula la dirección de un buffer local en el stack `[esp+0x20]` 32 bytes (0x20) -> `stack_buffer`
5. <+71>: Guarda en lo alto del stack el valor de EAX (buffer de stdin()).
6. <+74>: Llama a `fgets()` -> `fgets(stack_buffer, 128, stdin)`
**IMPORTANTE**: `fgets` lee de la entrada estándar (stdin) y guarda el texto en el stack, empezando en la posición `esp+0x20`.

### **Líneas 79 y 81:**
```asm
0x080485b3 <+79>:	test   eax,eax
0x080485b5 <+81>:	je     0x804872c <main+456>
```

1. <+79>: Compara EAX con sigo mismo para testear que ha devuelto `fgets()` -> NULL (sin hay un error) o la dirección del `buffer`. Sirve para actualizar los "flags" (indicadores) de la CPU.
2. <+81>: Si EAX es 0 se activa `Zero Flag (ZF = 1)` y si es cualquier otra cosas (ZF = 0). *`JE`* (Jump if Equal). Saltar si el Zero Flag está activado (ZF = 1). Si `fgets()` falla o pulsamos `Control + D`, salta a la ĺinea <+456> que es el final del programa.

### **Líneas 87, 91, 93, 98, 103, 105 y 107: Zona del Parseo**
```asm
0x080485bb <+87>:	lea    eax,[esp+0x20]
0x080485bf <+91>:	mov    edx,eax
0x080485c1 <+93>:	mov    eax,0x8048819
0x080485c6 <+98>:	mov    ecx,0x5
0x080485cb <+103>:	mov    esi,edx
0x080485cd <+105>:	mov    edi,eax
0x080485cf <+107>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
```

1. <+87>: Calcula la dirección de un buffer local en el stack `[esp+0x20]` 32 bytes (0x20) -> `stack_buffer`.
2. <+91>: Copia lo leido por `fgtest()` en EDX.
3. <+93>: Carga en EAX la dirección `0x8048819` que resulta ser un string con la palabra `auth `:
```bash
(gdb) x/s 0x8048819
0x8048819:	 "auth "
```
4. <+98>: Almacena en ECX 5 bytes (`int count = 5`): cuando hay un `mov ecx, 0x5` justo antes de un `repz cmps`, ese 0x5 es el contador.
5. <+103>: Carga en ESI lo leido por `fgets()`
6. <+105>: Carga en EDI el string `auth `
7. <+107>: Compara byte a byte lo que ha leido con el string `auth ` hasta un máximmo de 5.
**IMPORTANTE:** La instrucción `repz cmps` significa *"repite la comparación mientras sean iguales"*. ¿Cuántas veces? Las veces que diga `ECX`. En este caso, compara exactamente los 5 primeros bytes (la palabra "auth " tiene 5 caracteres contando el espacio).

### **Líneas 109, 112, 115, 117, 119, 121, 124 y 126:**
```asm
0x080485d1 <+109>:	seta   dl
0x080485d4 <+112>:	setb   al
0x080485d7 <+115>:	mov    ecx,edx
0x080485d9 <+117>:	sub    cl,al
0x080485db <+119>:	mov    eax,ecx
0x080485dd <+121>:	movsx  eax,al
0x080485e0 <+124>:	test   eax,eax
0x080485e2 <+126>:	jne    0x8048642 <main+222>
```

1. <+109> y <+112>: Usa `seta` (Set if Above) y `setb` (Set if Below). Básicamente, guarda un 1 en `DL` si la string es "mayor" y un 1 en `AL` si es "menor".
2. <+115> y <+117>: Mueve los valores y hace una resta:
- Si son iguales: **1 − 1 = 0 o 0 − 0 = 0**.
- Si son distintas: El resultado será distinto de 0.
3. <+119>: Carga el resultado en EAX
4. <+121>: Usa `movsx` para extender el signo.
5. <+124>: Testea si EAX es `0` -> si es `0`se activa `flag = 1`. Escribimos `auth .`
6. <+126>: `JNE` (Jump if Not Equal). Si no es `auth ` salta a la linea <+222> (flag = 1).

### **Líneas 128, 135, 140, 145, 150, 156 y 160:**
```asm
0x080485e4 <+128>:	mov    DWORD PTR [esp],0x4
0x080485eb <+135>:	call   0x8048470 <malloc@plt>
0x080485f0 <+140>:	mov    ds:0x8049aac,eax
0x080485f5 <+145>:	mov    eax,ds:0x8049aac
0x080485fa <+150>:	mov    DWORD PTR [eax],0x0
0x08048600 <+156>:	lea    eax,[esp+0x20]
0x08048604 <+160>:	add    eax,0x5
```

1. <+128>: Pone el lo alto del stack 4 bytes como argumento de malloc
2. <+135>: Llama a `malloc` y reserva 4 bytes -> (malloc(4)).
3. <+140>: Guarda la dirección que devolvió `malloc` en la variable global `auth` (0x8049aac).
4. <+145>: Actualiza EAX con el contenido de la global `auth`
5. <+150>: Inicialza el contenido de esa dirección a 0 -> `auth[0] = 0`
6. <+156>: Recupera en EAX el `stack_buffer` [esp+0x20]
7. <+160>: Suma 5 bytes para saltarse la palabra `auth`. 

### **Líneas 163, 171, 173, 178, 182 y 184: (Implntación del compilar de un strlen)**
```asm
0x08048607 <+163>:	mov    DWORD PTR [esp+0x1c],0xffffffff
0x0804860f <+171>:	mov    edx,eax
0x08048611 <+173>:	mov    eax,0x0
0x08048616 <+178>:	mov    ecx,DWORD PTR [esp+0x1c]
0x0804861a <+182>:	mov    edi,edx
0x0804861c <+184>:	repnz scas al,BYTE PTR es:[edi]
```

1. <+163>: Pone 0xffffffff (-1) en el stack. Se usa como contador inicial para ECX.
2. <+171>: Carga EAX (`stack_buffer`) en EDX
3. <+173>: Carga un 0 (0x0) al final del string (`stack_buffer`)
4. <+178>: Carga el puntero del stack [esp+0x1c] en ECX
5. <+182>: Copia EDX (`stack_buffer`) en el registro EDI
6. <+184>: Recorre el EDI (`stack_buffer`). `repnz scas` busca el valor 0x00 (el final de la string). Como ECX empezó en -1, al terminar, la diferencia nos dice cuántos caracteres hay.

### **Líneas 186, 188, 190,193, 196, 198, 202, 205, 210, 214 y 217:**
```asm
0x0804861e <+186>:	mov    eax,ecx
0x08048620 <+188>:	not    eax
0x08048622 <+190>:	sub    eax,0x1
0x08048625 <+193>:	cmp    eax,0x1e
0x08048628 <+196>:	ja     0x8048642 <main+222>
0x0804862a <+198>:	lea    eax,[esp+0x20]
0x0804862e <+202>:	lea    edx,[eax+0x5]
0x08048631 <+205>:	mov    eax,ds:0x8049aac
0x08048636 <+210>:	mov    DWORD PTR [esp+0x4],edx
0x0804863a <+214>:	mov    DWORD PTR [esp],eax
0x0804863d <+217>:	call   0x8048460 <strcpy@plt>
```

1. <+186>: Recupera en EAX el valor de ECX (que tiene el resultado del contador del scas).
2. <+188> y <+190>: Esto es una operación lógica para convertir el contador de la CPU en un número positivo que nosotros podamos entender. El resultado final en EAX es cuántos caracteres escribimos después de "auth ".
3. <+190>: Resta un byte(retrocede el puntero)
4. <+193>: Compara el tamaño de EAX con el núemero 30 (0x1e)
5. <+196>: `JA` (Jump if Above). Si el texto mide mas de 30 caracteres salta la línea 222 y no hace nada.
6. <+198>: Si mide 30 o  menos, sigue y carga en EAX la dirección de `stack_buffer` [esp+0x20]
7. <+202>: Le suma 5 a esa dirección para saltarse la palabra `"auth "` en el string y lo guarda en EDX (este es el **ORIGEN**).
8. <+205>: Actualiza EAX con el puntero de la variable global `auth`. (la dirección del heap que nos dio **malloc(4)** antes). Este es el **DESTINO**.
```bash
(gdb) x/s 0x8049aac
0x8049aac <auth>:	 ""
```
9. <+210> y <+214>: Prepara los argumentos en el stack para la función `strcpy()`:
- [esp] es el destino.
- [esp+0x4] es el origen.
10. <+217>: Llama a la finción `strcpy()`: strcpy([esp], [esp+0x4]).
**IMPORTANTE**: Va a copiar hasta 30 bytes en un malloc de 4 bytes. **AQUÍ ESTÁ EL: Heap Overflow**

### **Líneas de la 22 hasta la 216.Zona del Segundo Parseo (reset):**
```asm
0x08048642 <+222>:	lea    eax,[esp+0x20]
0x08048646 <+226>:	mov    edx,eax
0x08048648 <+228>:	mov    eax,0x804881f
0x0804864d <+233>:	mov    ecx,0x5
0x08048652 <+238>:	mov    esi,edx
0x08048654 <+240>:	mov    edi,eax
0x08048656 <+242>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
```
1. <+222>: Vuelve a cargar en EAX la dirección de `stack_buffer` (esp+0x20).
2. <+226>: Copia esa dirección en EDX
3. <+228>: Carga en EAX la dirección `0x804881f` que es un string con la palabra: "reset":
```bash
(gdb) x/s 0x804881f
0x804881f:	 "reset"
``` 
4. <+233>: Pone un 5 en ECX (el contador). Esta vez va a comparar 5 letras.
5. <+238> y <+240>: Prepara los registros de cadena: ESI apunta a lo que escribimos y EDI apunta a la palabra `"reset"`.
6. <+242>: Compara byte a byte nuestro texto con `"reset"` hasta un máximo de 5. 

### **Líneas 244, 247, 250, 252, 254 y 256: ¿Es "reset"?**
```asm
0x08048658 <+244>:	seta   dl
0x0804865b <+247>:	setb   al
0x0804865e <+250>:	mov    ecx,edx
0x08048660 <+252>:	sub    cl,al
0x08048662 <+254>:	mov    eax,ecx
0x08048664 <+256>:	movsx  eax,al
```

1. <+244> a <+256>: Es lo mismo de antes `(seta, setb, sub)`. Básicamente calcula la diferencia. Si las strings son iguales, EAX terminará siendo 0.

### **Líneas 259 y 261: El Salto del comando reset**
```asm
0x08048667 <+259>:	test   eax,eax
0x08048669 <+261>:	jne    0x8048678 <main+276>
```

1. <+259>: Testea si EAX es 0 -> si es `0`se activa `flag = 1`.
2. <+261>: `JNE` (Jump if Not Equal). Si no es `reset` salta a la linea <+276> (flag = 1). para probar con el siguiente comando ("service").

### **Líneas 263, 268 y 271:**
```asm
0x0804866b <+263>:	mov    eax,ds:0x8049aac
0x08048670 <+268>:	mov    DWORD PTR [esp],eax
0x08048673 <+271>:	call   0x8048420 <free@plt>
```

1. Si es `reset` la palabra escrita carga en EAX el puntero de la variable global `auth`:
```bash
(gdb) x/s 0x8049aac
0x8049aac <auth>:	 ""
```