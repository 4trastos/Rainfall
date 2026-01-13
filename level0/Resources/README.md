# **ANÁLISIS COMPLETO LINEA POR LINEA - NIVEL level0**

```
Dump of assembler code for function main:
   0x08048ec0 <+0>:     push   ebp
   0x08048ec1 <+1>:	    mov    ebp,esp
   0x08048ec3 <+3>:	    and    esp,0xfffffff0
   0x08048ec6 <+6>:	    sub    esp,0x20
   0x08048ec9 <+9>:	    mov    eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>:	add    eax,0x4
   0x08048ecf <+15>:	mov    eax,DWORD PTR [eax]
   0x08048ed1 <+17>:	mov    DWORD PTR [esp],eax
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    eax,0x1a7
   0x08048ede <+30>:	jne    0x8048f58 <main+152>
   0x08048ee0 <+32>:	mov    DWORD PTR [esp],0x80c5348
   0x08048ee7 <+39>:	call   0x8050bf0 <strdup>
   0x08048eec <+44>:	mov    DWORD PTR [esp+0x10],eax
   0x08048ef0 <+48>:	mov    DWORD PTR [esp+0x14],0x0
   0x08048ef8 <+56>:	call   0x8054680 <getegid>
   0x08048efd <+61>:	mov    DWORD PTR [esp+0x1c],eax
   0x08048f01 <+65>:	call   0x8054670 <geteuid>
   0x08048f06 <+70>:	mov    DWORD PTR [esp+0x18],eax
   0x08048f0a <+74>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f0e <+78>:	mov    DWORD PTR [esp+0x8],eax
   0x08048f12 <+82>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f16 <+86>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f1a <+90>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f1e <+94>:	mov    DWORD PTR [esp],eax
   0x08048f21 <+97>:	call   0x8054700 <setresgid>
   0x08048f26 <+102>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f2a <+106>:	mov    DWORD PTR [esp+0x8],eax
   0x08048f2e <+110>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f32 <+114>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f36 <+118>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f3a <+122>:	mov    DWORD PTR [esp],eax
   0x08048f3d <+125>:	call   0x8054690 <setresuid>
   0x08048f42 <+130>:	lea    eax,[esp+0x10]
   0x08048f46 <+134>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f4a <+138>:	mov    DWORD PTR [esp],0x80c5348
   0x08048f51 <+145>:	call   0x8054640 <execv>
   0x08048f56 <+150>:	jmp    0x8048f80 <main+192>
   0x08048f58 <+152>:	mov    eax,ds:0x80ee170
   0x08048f5d <+157>:	mov    edx,eax
   0x08048f5f <+159>:	mov    eax,0x80c5350
   0x08048f64 <+164>:	mov    DWORD PTR [esp+0xc],edx
   0x08048f68 <+168>:	mov    DWORD PTR [esp+0x8],0x5
   0x08048f70 <+176>:	mov    DWORD PTR [esp+0x4],0x1
   0x08048f78 <+184>:	mov    DWORD PTR [esp],eax
   0x08048f7b <+187>:	call   0x804a230 <fwrite>
   0x08048f80 <+192>:	mov    eax,0x0
   0x08048f85 <+197>:	leave  
   0x08048f86 <+198>:	ret    
End of assembler dump.
```

## **SECCIÓN 1: PRÓLOGO - PREPARACIÓN DEL STACK FRAME**

### **Línea 0: `push ebp`**
```
push ebp
```
- Guarda el valor actual de EBP en la pila (stack). Que es valor de la función (CPU) que llama a la siguinte (main). [ ebp + 0x00]
- EIP en [ebp + 0x04]. Indica a dónde volver cuando la función termine.
- Al guardar en el stack el valor del [ebp antoguo] el ESP se mueve 4 bytes hacia abajo

### **Línea 1: `mov ebp,esp`**
```
mov ebp,esp
```
- Copia el nuevo esp (del stack) en ebp para obtener un nuevo registro para la función.
- Ahora ESP apunta al inicio del "stack frame" de `main`.

### **Línea 3: `and esp,0xfffffff0`**
```
and esp,0xfffffff0
```
- Aplica operación AND bit a bit entre ESP y 0xfffffff0
- Alinea el stack a múltiplo de 16 bytes (alineación requerida por ABI System V)
- 0xfffffff0 = 11111111111111111111111111110000 en binario
- Los últimos 4 bits de ESP se ponen a 0 (ej: 0xbffff7c9 → 0xbffff7c0)

### **Línea 6: `sub esp,0x20`**
```
sub esp,0x20
```
- Reserva espacio para variables locales en el stack
- 32 bytes para uso de la función `main()`

---

## **SECCIÓN 2: OBTENER Y VERIFICAR ARGUMENTO**

### **Línea 9: `mov eax,DWORD PTR [ebp+0xc]`**
```
mov eax,DWORD PTR [ebp+0xc]
```
- Carga en EAX el contenido de la dirección `EBP + 0xc` (12 bytes)
- **Contexto de stack**:
  - `[ebp+0]`: El EBP de la función que llamó a esta. (suelo anterior).
  - `[ebp+4]`: Dirección de retorno (EIP guardado)
  - `[ebp+8]`: `argc` (int argc)
  - `[ebp+0xc]`: `argv`(char **argv)
- `eax = argv`

### **Línea 12: `add eax,0x4`**
```
add eax,0x4
```
- Suma 4 a EAX
- En arquitectura 32-bit, los punteros son de 4 bytes
- Si `argv` apunta a `argv[0]`, entonces `argv + 4` apunta a `argv[1]`
- `eax = &argv[1]` (segundo argumento: ./level **423**) 

### **Línea 15: `mov eax,DWORD PTR [eax]`**
```
mov eax,DWORD PTR [eax]
```
- Carga en EAX el contenido de la dirección apuntada por EAX
- **DWORD PTR**: Indica que estamos accediendo a 4 bytes (un puntero)
- `eax = argv[1]` (el string del primer argumento real)

### **Línea 17: `mov DWORD PTR [esp],eax`**
```
mov DWORD PTR [esp],eax
```
- Guarda EAX en la dirección apuntada por ESP (tope del stack)
- Prepara el argumento para la función `atoi()`
- `[argv[1]]` en el tope

### **Línea 20: `call 0x8049710 <atoi>`**
```
call 0x8049710 <atoi>
```
- Llama a la función `atoi()` en dirección 0x8049710
- **Efectos**:
  1. Guarda EIP actual (dirección de retorno) en el stack
  2. Salta a 0x8049710
  3. `atoi()` convierte el string a entero
  4. Por convención x86, el resultado se devuelve en EAX

### **Línea 25: `cmp eax,0x1a7`**
```
cmp eax,0x1a7
```
- Compara EAX con 0x1a7 (423 decimal)
- **Cálculo**: `eax - 0x1a7`, pero sin guardar resultado
- ZF (Zero Flag) = 1 si son iguales, 0 si diferentes

### **Línea 30: `jne 0x8048f58 <main+152>`**
```
jne 0x8048f58 <main+152>
```
- Salta a dirección 0x8048f58 si NO son iguales (jne = Jump if Not Equal)
- Si `atoi(argv[1]) != 423`, salta al camino de ERROR
- Continúa con la siguiente instrucción

---

## **SECCIÓN 3: CAMINO CORRECTO (ARGUMENTO = 423)**

### **Línea 32: `mov DWORD PTR [esp],0x80c5348`**
```
mov DWORD PTR [esp],0x80c5348
```
- *Guarda la constante 0x80c5348 en el tope del stack
- **¿Qué es 0x80c5348?**: Dirección de un string (probablemente "/bin/sh")
- **Stack ahora**: `[0x80c5348]` en el tope (argumento para strdup)

### **Línea 39: `call 0x8050bf0 <strdup>`**
```
call 0x8050bf0 <strdup>
```
- Llama a `strdup()` para duplicar el string
- String en dirección 0x80c5348
- EAX = puntero al string duplicado

### **Línea 44: `mov DWORD PTR [esp+0x10],eax`**
```
mov DWORD PTR [esp+0x10],eax
```
- Guarda el resultado de strdup en `[esp+0x10]`
- **Stack offset**: esp+0x10 = esp + 16 bytes
- Guardar el string duplicado como `argv[0]` para execv

### **Línea 48: `mov DWORD PTR [esp+0x14],0x0`**
```
mov DWORD PTR [esp+0x14],0x0
```
- Guarda 0 (NULL) en `[esp+0x14]`
- **Stack offset**: esp+0x14 = esp + 20 bytes
- Terminar el array de argumentos con NULL (`argv[1] = NULL`)

---

## **SECCIÓN 4: GESTIÓN DE PRIVILEGIOS**

### **Línea 56: `call 0x8054680 <getegid>`**
```
call 0x8054680 <getegid>
```
- Obtiene el GID efectivo (Effective Group ID)
- EAX = GID efectivo del proceso

### **Línea 61: `mov DWORD PTR [esp+0x1c],eax`**
```
mov DWORD PTR [esp+0x1c],eax
```
- Guarda el GID en `[esp+0x1c]` (stack+28 bytes)

### **Línea 65: `call 0x8054670 <geteuid>`**
```
call 0x8054670 <geteuid>
```
- Obtiene el UID efectivo (Effective User ID)
- EAX = UID efectivo del proceso

### **Línea 70: `mov DWORD PTR [esp+0x18],eax`**
```
mov DWORD PTR [esp+0x18],eax
```
- Guarda el UID en `[esp+0x18]` (stack+24 bytes)

---

## **SECCIÓN 5: CONFIGURACIÓN DE PRIVILEGIOS**

### **Líneas 74-97: `setresgid(gid, gid, gid)`**
```
0x08048f0a <+74>:	mov    eax,DWORD PTR [esp+0x1c]   ; EAX = GID
0x08048f0e <+78>:	mov    DWORD PTR [esp+0x8],eax    ; 3er arg = saved GID
0x08048f12 <+82>:	mov    eax,DWORD PTR [esp+0x1c]   ; EAX = GID otra vez
0x08048f16 <+86>:	mov    DWORD PTR [esp+0x4],eax    ; 2do arg = effective GID
0x08048f1a <+90>:	mov    eax,DWORD PTR [esp+0x1c]   ; EAX = GID otra vez
0x08048f1e <+94>:	mov    DWORD PTR [esp],eax        ; 1er arg = real GID
0x08048f21 <+97>:	call   0x8054700 <setresgid>      ; setresgid(gid, gid, gid)
```
- **Propósito**: Establece real, efectivo y saved GID al mismo valor
- **¿Por qué?**: Asegurar que todos los IDs de grupo sean consistentes

### **Líneas 102-125: `setresuid(uid, uid, uid)`**
```
0x08048f26 <+102>:	mov    eax,DWORD PTR [esp+0x18]  ; EAX = UID
0x08048f2a <+106>:	mov    DWORD PTR [esp+0x8],eax   ; 3er arg = saved UID
0x08048f2e <+110>:	mov    eax,DWORD PTR [esp+0x18]  ; EAX = UID otra vez
0x08048f32 <+114>:	mov    DWORD PTR [esp+0x4],eax   ; 2do arg = effective UID
0x08048f36 <+118>:	mov    eax,DWORD PTR [esp+0x18]  ; EAX = UID otra vez
0x08048f3a <+122>:	mov    DWORD PTR [esp],eax       ; 1er arg = real UID
0x08048f3d <+125>:	call   0x8054690 <setresuid>     ; setresuid(uid, uid, uid)
```
- **Propósito**: Establece real, efectivo y saved UID al mismo valor
- **¿Por qué?**: Asegurar que todos los IDs de usuario sean consistentes

---

## **SECCIÓN 6: EJECUCIÓN DEL PROGRAMA**

### **Línea 130: `lea eax,[esp+0x10]`**
```
lea eax,[esp+0x10]
```
- Load Effective Address - Calcula dirección de `[esp+0x10]`
- **LEA vs MOV**: LEA calcula direcciones, MOV mueve datos
- `eax = &stack[esp+0x10]` (dirección donde guardamos strdup)

### **Línea 134: `mov DWORD PTR [esp+0x4],eax`**
```
mov DWORD PTR [esp+0x4],eax
```
- Guarda EAX en `[esp+0x4]`
- Segundo argumento para execv = array de argumentos

### **Línea 138: `mov DWORD PTR [esp],0x80c5348`**
```
mov DWORD PTR [esp],0x80c5348
```
- Guarda 0x80c5348 en `[esp]`
- Primer argumento para execv = path del programa

### **Línea 145: `call 0x8054640 <execv>`**
```
call 0x8054640 <execv>
```
- Llama a `execv(path, argv)`
- **Parámetros**: 
  - `path = 0x80c5348` (probablemente "/bin/sh")
  - `argv = [strdup(path), NULL]`
- Reemplaza el proceso actual por una shell

### **Línea 150: `jmp 0x8048f80 <main+192>`**
```
jmp 0x8048f80 <main+192>
```
- Salto incondicional al final de la función
- Si execv tiene éxito, no retorna. Esta línea es por si falla

---

## **SECCIÓN 7: CAMINO DE ERROR (ARGUMENTO ≠ 423)**

### **Línea 152: `mov eax,ds:0x80ee170`**
```
mov eax,ds:0x80ee170
```
- Carga el contenido de dirección 0x80ee170 en EAX
- **ds:**:
Prefijo de segmento de datos
- **¿Qué es 0x80ee170?**: Probablemente `stderr` (FILE*)

### **Líneas 157-164: Preparar argumentos para fwrite**
```
0x08048f5d <+157>:	mov    edx,eax                   ; EDX = stderr
0x08048f5f <+159>:	mov    eax,0x80c5350             ; EAX = dirección del string de error
0x08048f64 <+164>:	mov    DWORD PTR [esp+0xc],edx   ; 4to arg = stderr
```
- **0x80c5350**: Dirección del string "Error\n" probablemente

### **Líneas 168-184: Continuar preparación fwrite**
```
0x08048f68 <+168>:	mov    DWORD PTR [esp+0x8],0x5   ; 3er arg = longitud (5 bytes)
0x08048f70 <+176>:	mov    DWORD PTR [esp+0x4],0x1   ; 2do arg = tamaño elemento (1 byte)
0x08048f78 <+184>:	mov    DWORD PTR [esp],eax       ; 1er arg = buffer (string "Error")
```
- **fwrite(buffer, size, count, stream)**:
  - buffer = "Error"
  - size = 1 byte por elemento
  - count = 5 elementos (5 caracteres)
  - stream = stderr

### **Línea 187: `call 0x804a230 <fwrite>`**
```
call 0x804a230 <fwrite>
```
- Escribe "Error" en stderr

---

## **SECCIÓN 8: LIMPIEZA Y RETORNO**

### **Línea 192: `mov eax,0x0`**
```
mov eax,0x0
```
- Pone 0 en EAX
- Valor de retorno de la función (0 = éxito)

### **Línea 197: `leave`**
```
leave
```
- **Equivalente a**:
  ```
  mov esp,ebp    ; Restaura ESP
  pop ebp        ; Restaura EBP antiguo
  ```
- Deshace el stack frame de la función

### **Línea 198: `ret`**
```
ret
```
- **Acción**: Return - Saca dirección de retorno del stack y salta allí
- **Fin**: Termina ejecución de `main()`

---


# Bonus A). El Registro EBP/RBP y la Pila

Aclaración técnica: El **registro** EBP (en 32 bits) o RBP (en 64 bits) es una "caja" dentro de la CPU que guarda **una sola dirección de memoria**.

(los bloques -0x4, 0x0, +0x4...) no es el registro en sí, sino la **Memoria RAM (el Stack)** organizada alrededor de la dirección que apunta ese registro.

---

# Bonus B). Mapa del Stack para `ft_test(int x, char c)`

```bash

int ft_test(int x, char c) {

    int j = 0;
    return = j;
}

```

En una arquitectura de **32 bits** (como la de *Rainfall*), las cosas se apilan de **4 en 4 bytes**. Así es como se vería la RAM cuando estás dentro de la función:

| Dirección en RAM | Contenido | Explicación |
| --- | --- | --- |
| **`[EBP + 0x0c]`** | `char c` | **Argumento 2:** (Ocupa 4 bytes en el stack por alineación). |
| **`[EBP + 0x08]`** | `int x` | **Argumento 1:** El primer valor enviado. |
| **`[EBP + 0x04]`** | **Retorno** | **DIRECCIÓN DE RETORNO:** La "nota" para que el EIP sepa volver. |
| **`[EBP + 0x00]`** | **EBP Viejo** | **El "Suelo" anterior:** El EBP de la función que llamó a esta. |
| **`[EBP - 0x04]`** | `int j` | **Variable Local:** Tu `int j = 0`. |

---

