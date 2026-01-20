#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void    p(void)
{
    // 0x080484d7 <+3>:	    sub    esp,0x68 (104 bytes reservados)
    // 0x080484e7 <+19>:	lea    eax,[ebp-0x4c] (76 bytes de distancia al EBP)
    char            buffer[64];
    unsigned int    ret;

    fflush(stdout);
    gets(buffer);

    // 0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]
    // Esta parte simula la protección "anti-stack" del binario
    ret = __builtin_return_address(0);

    // 0x080484fb <+39>:	and    eax,0xb0000000
    if ((ret & 0xb0000000) == 0xb0000000)
    {
        printf("(%p)\n", (void *)ret);
        exit(1);
    }
    
    // 0x0804852d <+89>:	call   0x80483f0 <puts@plt>
    puts(buffer);

    // 0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
    // Aquí es donde se crea la copia en el HEAP que explotamos
    strdup(buffer);
}

int main()
{
    p();
    return (0);
}