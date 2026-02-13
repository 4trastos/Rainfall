#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void    n(void)
{
    system("/bin/cat /home/user/level7/.pass");
    return;
}

char   *m(void)
{
    puts("Nope\n");
    return;
}

int main (int argc, char **argv)
{
    char *buffer;
    char *pointer;

    buffer = malloc(64);
    pointer = malloc(4);

    pointer = m();
    if (argc == 1)
        m();

    strcpy(buffer, argv[1]);

    return (0);
}