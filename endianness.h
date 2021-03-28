#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <stdint.h>
#include <stdio.h>

int16_t leer_int16_little_endian(FILE *f);                  // Lee un entero de 16 bits en codificacion Little Endian de un archivo f abierto en modo lectura.
void escribir_int16_little_endian(FILE *f, int16_t v);      // Escribe en un archivo f abierto en modo escritura un entero de 16 bits en codificacion Little Endian.
int32_t leer_int32_little_endian(FILE *f);                  // Lee un entero de 32 bits en codificacion Little Endian de un archivo f abierto en modo lectura.
void escribir_int32_little_endian(FILE *f, int32_t v);      // Escribe en un archivo f abierto en modo escritura un entero de 32 bits en codificacion Little Endian.

#endif