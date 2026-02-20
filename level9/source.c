#include <iostream>
#include <cstring>
#include <cstdlib>

class N {
public:
    char annotation[100]; // Buffer vulnerable
    int value;

    N(int v) : value(v) {}

    // Función virtual: crea una entrada en la vtable
    virtual int operator+(N &rhs) {
        return this->value + rhs.value;
    }

    virtual int operator-(N &rhs) {
        return this->value - rhs.value;
    }

    // Método que causa el desbordamiento
    void setAnnotation(char *str) {
        // ERROR: memcpy no limita el tamaño basándose en el buffer de destino
        // Copia tantos bytes como tenga el argumento de entrada
        memcpy(this->annotation, str, strlen(str));
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        exit(1);
    }

    // Se reservan dos objetos en el Heap (aprox. 108-112 bytes de distancia)
    N *obj1 = new N(5);
    N *obj2 = new N(6);

    N &ref1 = *obj1;
    N &ref2 = *obj2;

    // Se llena el primer objeto con el argumento del usuario
    obj1->setAnnotation(argv[1]);

    // Llamada virtual: el programa busca el vptr de obj2 para sumar.
    // Si hemos pisado obj2, el programa saltará a donde nosotros queramos.
    return ref2 + ref1;
}
