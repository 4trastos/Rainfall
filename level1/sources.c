#include <stdio.h>
#include <stdlib.h>

void    run(void)
{
    fwrite("Good... Wait what?\n", 1, 19, stdout);
    system("/bin/sh");
}

int main(void)
{
    char buffer[64];   // Reservamos 64 bytes. 0x50 - 0x10 = 64 

    gets(buffer);
    return (0);
}


