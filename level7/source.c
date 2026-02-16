#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_struct
{
    int id;             // ← 4 bytes con valor 1 o 2
    char * buffer;      // ← 4 bytes con el puntero
}   t_struct;

char c[68];             // ← Variable global en 0x8049960

void m(void)
{
    printf("%s", c);    // ← imprime el contenido de la variable global
}

int main(int argc, char **argv)
{
    t_struct *struct_01 = NULL;
    t_struct *struct_02 = NULL;
    FILE *file = NULL;

    struct_01 = malloc(8);
    struct_02 = malloc(8);

    struct_01->id = 1;                  // ← <+29>
    struct_02->id = 2;                  // ← <+76>
    
    struct_01->buffer = malloc(8);      // ← <+127>
    struct_02->buffer = malloc(8);      // ← <+156>

    strcpy(struct_01->buffer, argv[1]);
    strcpy(struct_02->buffer, argv[2]);

    file = fopen("/home/user/level8/.pass", "r");   // ← <+178>
    fgets(c, 68, file);                             // ← <+202> Guarda en variable global 0x8049960

    puts("~~");                                     // ← <+214>

    return (0);
}