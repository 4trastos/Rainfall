# **GUÍA COMPLETA GDB PARA RAINFALL/SNOWCRASH**

## **1. INICIAR DEBUGGING**

### **Formas de iniciar:**
```bash
# Básico
gdb ./level0

# Con TUI (Text User Interface)
gdb -tui ./level0

# Con argumentos
gdb --args ./level0 "argumento1"

# Con breakpoint en main desde inicio
gdb -ex "break main" -ex "run" ./level0
```

## **2. COMANDOS ESENCIALES DE EJECUCIÓN**

### **Control de ejecución:**
```bash
(gdb) run [args]              # Iniciar programa con argumentos
(gdb) start                   # Iniciar y parar en main()
(gdb) continue / c            # Continuar ejecución
(gdb) next / n                # Siguiente línea (sin entrar funciones)
(gdb) step / s                # Siguiente línea (entra en funciones)
(gdb) nexti / ni              # Siguiente instrucción assembler
(gdb) stepi / si              # Siguiente instrucción (entra en calls)
(gdb) finish                  # Ejecutar hasta salir de función actual
(gdb) until <dirección>       # Ejecutar hasta dirección específica
```

## **3. BREAKPOINTS Y WATCHPOINTS**

### **Puntos de interrupción:**
```bash
(gdb) break main              # Break en función main()
(gdb) break *0x08048444       # Break en dirección específica
(gdb) break func.c:25         # Break en línea de archivo (si hay source)
(gdb) break printf            # Break en función printf()
(gdb) break if $eax == 0      # Break condicional
(gdb) info breakpoints        # Listar todos los breakpoints
(gdb) delete 1                # Eliminar breakpoint 1
(gdb) disable 2               # Desactivar breakpoint 2
(gdb) enable 2                # Activar breakpoint 2
```

### **Watchpoints (para variables):**
```bash
(gdb) watch *0x804a000        # Parar cuando se escriba en esta dirección
(gdb) watch variable          # Parar cuando variable cambie
(gdb) rwatch variable         # Parar cuando se lea variable
(gdb) awatch variable         # Parar cuando se lea o escriba
```

## **4. INSPECCIÓN DE MEMORIA Y REGISTROS**

### **Registros:**
```bash
(gdb) info registers          # Ver TODOS los registros
(gdb) info registers eax      # Ver registro específico
(gdb) print $eax              # Ver valor de EAX
(gdb) print $eip              # Instruction Pointer
(gdb) print/x $eax            # En hexadecimal
(gdb) set $eax = 0x10         # Cambiar valor de registro
```

### **Memoria:**
```bash
(gdb) x/10x $esp              # Ver 10 words en hex desde ESP
(gdb) x/20x 0x804a000         # Ver 20 words en dirección
(gdb) x/10i $eip              # Ver 10 instrucciones desde EIP
(gdb) x/s 0x804a010           # Ver como string
(gdb) x/xw 0x804a000          # Ver como word (4 bytes)
(gdb) x/10gx $rsp             # Para 64-bit (8 bytes cada uno)
```

### **Formatos de examen (x/):**
- **x** - hexadecimal
- **d** - decimal con signo
- **u** - decimal sin signo
- **o** - octal
- **t** - binario
- **f** - float
- **a** - dirección
- **i** - instrucción
- **s** - string
- **c** - carácter

## **5. STACK Y LLAMADAS A FUNCIÓN**

### **Stack tracing:**
```bash
(gdb) backtrace / bt          # Ver call stack
(gdb) backtrace full          # Stack con variables locales
(gdb) frame 0                 # Cambiar al frame 0 del stack
(gdb) info frame              # Info del frame actual
(gdb) info locals             # Variables locales del frame
(gdb) info args               # Argumentos de la función
(gdb) print $ebp              # Base pointer actual
(gdb) print $esp              # Stack pointer actual
```

## **6. TUI (TEXT USER INTERFACE) - PARA RAINFALL**

### **Modos de visualización:**
```bash
gdb -tui ./level0            # Iniciar con TUI

# Dentro de GDB:
(gdb) layout asm             # Solo assembler
(gdb) layout regs            # Solo registros
(gdb) layout src             # Solo código fuente (si disponible)
(gdb) layout split           # Split: assembler + source/regs
(gdb) focus cmd              # Cambiar foco a línea de comandos
(gdb) refresh                # Refrescar pantalla
(gdb) Ctrl+L                 # Refrescar pantalla (atajo)
(gdb) Ctrl+X, A              # Alternar TUI ON/OFF
```

### **Mi configuración TUI para Rainfall:**
```bash
# En ~/.gdbinit
set disassembly-flavor intel
set pagination off

define tuirain
  layout split
  layout regs
  focus cmd
end
```

## **7. PARA BINARIOS SIN SOURCE (Rainfall)**

### **Configuración assembler:**
```bash
(gdb) set disassembly-flavor intel   # Sintaxis Intel (más legible)
(gdb) disas main                     # Desensamblar main
(gdb) disas function_name            # Desensamblar función
(gdb) info functions                 # Listar todas las funciones
```

### **Breakpoints en assembler:**
```bash
(gdb) break *main                    # Break al inicio de main
(gdb) break *0x08048444              # Break en dirección
(gdb) break *main+10                 # Break 10 bytes después de main
```

## **8. EXAMINAR BINARIO ANTES DE EJECUTAR**

### **Información del binario:**
```bash
(gdb) info files                     # Secciones del binario
(gdb) info functions                 # Todas las funciones
(gdb) info proc mappings             # Mapeo de memoria
(gdb) shell readelf -a ./level0      # Info ELF (desde gdb)
```

## **9. COMANDOS PARA EXPLOTACIÓN**

### **Para format strings:**
```bash
(gdb) break printf
(gdb) run "%x %x %x"
(gdb) x/20x $esp                    # Ver stack antes de printf
```

### **Para buffer overflows:**
```bash
(gdb) pattern create 100            # Crear pattern
(gdb) run $(python -c 'print "A"*100')
(gdb) info registers eip            # Ver EIP después del crash
(gdb) x/wx $esp                     # Ver lo que hay en ESP
```

## **10. SCRIPTING Y AUTOMATIZACIÓN**

### **Comandos en lote:**
```bash
# Archivo script.gdb
set args "test"
break main
run
x/10i $pc
info registers
continue

# Ejecutar
gdb -x script.gdb ./level0
```

### **Comandos útiles en .gdbinit:**
```bash
# ~/.gdbinit para Rainfall
set disassembly-flavor intel
set pagination off
define hook-stop
  x/5i $pc
  info registers eax ebx ecx edx
end
```

## **11. EJEMPLO COMPLETO PARA LEVEL0 RAINFALL**

```bash
# Paso 1: Analizar binario
gdb ./level0
(gdb) info functions
(gdb) disas main

# Paso 2: Configurar TUI
(gdb) set disassembly-flavor intel
(gdb) layout split
(gdb) layout regs

# Paso 3: Poner breakpoints
(gdb) break *main
(gdb) break *strcmp
(gdb) break *printf

# Paso 4: Ejecutar con argumento
(gdb) run "AAAA"

# Paso 5: Depurar paso a paso
(gdb) ni              # next instruction
(gdb) si              # step instruction (entra en calls)
(gdb) x/10x $esp      # ver stack
(gdb) print $eax      # ver registro eax

# Paso 6: Continuar hasta próximo break
(gdb) continue
```

## **12. ALIAS ÚTILES**

```bash
(gdb) define ri       # Run con argumentos de inyección
> set args "; /bin/sh ;"
> run
> end

(gdb) define xs       # Examinar stack
> x/32wx $esp
> end

(gdb) define xr       # Ver registros importantes
> info registers eax ebx ecx edx esi edi ebp esp eip
> end
```

## **13. SALIR Y GUARDAR**

```bash
(gdb) quit / q        # Salir de gdb
(gdb) save breakpoints breaks.txt  # Guardar breakpoints
(gdb) source breaks.txt           # Cargar breakpoints
(gdb) shell clear     # Limpiar terminal (sin salir de gdb)
```