#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char    *service = NULL;                                    // ← 0x08049ab0
char    *auth = NULL;                                       // ← 0x08049aac

int main(void)
{
    char    stack_buffer[128];                              // ← [esp+0x20]

    while (1)
    {
        printf("%p, %p \n", service, auth);

        if (fgets(stack_buffer, 128, stdin) == NULL)        // ← Línea <+74>
            break;

        if (strncmp(stack_buffer, "auth ", 5) == 0)
        {
            auth = malloc(4);                               // ← <+135>: Reserva espacio para el puntero
            if (auth == NULL)
                exit(1);
            memset(auth, 0, 4);                             // ← <+150>: Inicializa a cero

            // Líneas <+193> y <+217>: El Heap Overflow
            // Comprueba si el argumento tiene menos de 30 chars
            if (strlen(stack_buffer + 5) <= 30)
                strcpy(auth, stack_buffer + 5);
        }

        if (strncmp(stack_buffer, "reset", 5) == 0)
            free(auth);
        
        if (strncmp(stack_buffer, "service", 7) == 0)
            service = strdup(stack_buffer + 8);

        if (strncmp(stack_buffer, "login", 5) == 0)
        {
            // <+387>: EL ERROR CRÍTICO. 
            // Accede a la posición [auth + 32 bytes] para ver si es != 0
            if (*(int*)(auth + 32) != 0)
                system("/bin/sh");
            else
                printf("Password:\n");
        }
    }
    return (0);
}
