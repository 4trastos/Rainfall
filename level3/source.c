#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 0x0804988c: Dirección de la variable global 'm'
int m = 0;

void    v()
{
    // sub esp, 0x218           -> reserva de 536 bytes
    // lea eax, [ebp-0x208]     -> el buffer empieza 520 bytes antes de EBP
    char buffer[512];

    // mov DWORD PTR [esp+0x4], 0x200 (512 bytes)
    // mov eax, ds:0x8049860 (stdin)
    fgets(buffer, 512, stdin);

    // VULNERABILIDAD CRÍTICA:
    // call printf@plt 
    // printf sin string de formato
    // Debería ser: printf("%s", buffer);
    printf(buffer);

    // 0x080484da: mov eax, ds:0x804988c (carga el valor de 'm')
    // 0x080484df: cmp eax, 0x40 (64 decimal)
    if (m == 64)
    {
        // 0x08048507: call fwrite@plt 
        // Imprime "Wait what?!\n"
        fwrite("Wait what?!\n", 1, 12, stdout);
        
        // 0x08048513: call system@plt
        // 0x0804860d es la dirección de la cadena "/bin/sh"
        system("/bin/sh");
    }

}

int main()
{
    // Alineación del stack y llamada a 'v'
    v();
    return (0);
}