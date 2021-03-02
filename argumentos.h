#ifndef ARGUMENTOS_H
#define ARGUMENTOS_H

#include "imagen.h"

typedef struct argumentos argumentos_t;                                                         // TDA argumentos, contiene todos los argumentos pasados al programa con informacion respectiva de cada uno. Las operaciones (en caso de estar cargadas) estan enlistadas en el orden de [0,n).

argumentos_t *crear_vector_argumentos_vacio();                                                  // Crea un vector de argumentos vacio, devuelve NULL en caso de falla.
bool leer_argumentos(argumentos_t *argumentos, int argc, char *argv[]);                         // Lee todos los argumentos pasados y los carga al vector. En caso de falla devuelve false y deja cargado todo lo que pudo cargar.
imagen_t *cargar_imagen_desde_argumentos(argumentos_t *argumentos);                             // Crea un TDA imagen a partir del archivo cargado a traves de los parametros dentro del vector argumentos. En caso de  falla devuelve NULL.
size_t cantidad_operaciones(argumentos_t *argumentos);                                          // Devuelve las operaciones totales que fueron cargadas al vector.
bool es_recorte(argumentos_t *argumentos, size_t n);                                            // Determina si la operacion en posicion n es un recorte.
bool es_filtro(argumentos_t *argumentos, size_t n);                                             // Determina si la operacion en posicion n es un filtro.
bool es_espejo_horizontal(argumentos_t *argumentos, size_t n);                                  // Determina si la operacion en posicion n es un espejado horizontal.
bool es_espejo_vertical(argumentos_t *argumentos, size_t n);                                    // Determina si la operacion en posicion n es un espejado vertical.
imagen_t *realizar_recorte(argumentos_t *argumentos, size_t n, imagen_t *imagen);               // Realiza el recorte de orden n cargado en el vector argumentos a un TDA imagen y lo guarda en otro TDA imagen. En caso de falla o que la operacion n no sea un recorte devuelve NULL.
bool aplicar_filtro(argumentos_t *argumentos, size_t n, imagen_t *imagen);                      // Aplica el filtro  de orden n cargado en el vector argumentos a un TDA imagen. En caso de falla o que la operacion n no sea un filtro devuelve false.
bool escribir_imagen_desde_argumentos(argumentos_t *argumentos, imagen_t *imagen);              // Escribe un TDA imagen en el archivo de salida cargado en el vector argumentos. En caso de falla devuelve false.
void destruir_argumentos(argumentos_t *argumentos);                                             // Destruye el vector de argumentos.

#endif