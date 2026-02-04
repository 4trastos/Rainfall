#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void n()
{
    char buffer[520];

    fgets(buffer, 512, stdin);
    printf(buffer);              // -> VULNERABILIDAD CRÍTICA
    exit(1);                     // -> objetivo de GOT aquí
}

int main()
{
    n();
    return (0);
}