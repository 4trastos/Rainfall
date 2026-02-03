#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Varible global ubicada en 0x08049810
int m = 0;

void p(char *buffer)
{
    // call   0x8048340 <printf@plt>        -> VULNERABILIDAD CRÍTICA
    // printf sin string de formato
    // Debería ser: printf("%s", buffer);
    printf(buffer);
}

void n(void)
{
    // sub    esp,0x218                     -> reserva de 536 bytes
    // lea    eax,[ebp-0x208]               -> el buffer empieza 520 bytes antes de EBP
    char    buffer[520];

    // mov    DWORD PTR [esp+0x4],0x200     -> Lee hasta 512 bytes (0x200) de (stdin)
    // El tercer argumento (0x8049804) apunta a stdin
    fgets(buffer, 512, stdin);

    // call   0x8048444 <p>                 -> llamada a 'p'
    p(buffer);

    // cmp    eax,0x1025544                 -> Comparación de la variable global 'm'
    // 0x1025544 = 16930116 en decimal
    if (m == 16930116)
        system("/bin/sh -c 'cat /home/user/level5/.pass'");
}

int main()
{
    // and    esp,0xfffffff0                -> Alineación del stack
    // call   0x8048457 <n>                 -> llamada a 'n'
    n();
    return (0);
}
