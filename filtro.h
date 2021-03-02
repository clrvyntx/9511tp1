#ifndef FILTRO_H
#define FILTRO_H

#include "pixel.h"

typedef struct filtro filtro_t;                                                        // TDA filtro, contiene el filtro a aplicar, la cantidad de parametros y el valor de cada uno de ellos. Los filtros contenidos aqui tienen hasta 1 parametro como maximo.

filtro_t *crear_filtro(const char *nombre);                                            // Carga un filtro a partir de un nombre. Filtros validos: "invertir", "saturar", "gama", "brillo", "contraste", "mezclar", "sepia", "monocromo", "toaster", "valencia". En caso de no ser un nombre valido devuelve NULL.
bool cargar_parametro_a_filtro(filtro_t *filtro, const char* parametro);               // Carga un parametro en el filtro a traves de una cadena de caracteres con el parametro. En caso de que el parametro no sea correcto o que el filtro no necesite parametros devuelve false.
int numero_de_parametros(filtro_t *filtro);                                            // Devuelve la cantidad de parametros que utiliza el filtro.
pixel_t aplicar_filtro_en_pixel(pixel_t p, filtro_t *filtro);                          // Devuelve un TDA pixel con el filtro aplicado.
void destruir_filtro(filtro_t *filtro);                                                // Destruye el TDA filtro.

#endif