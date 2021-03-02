#ifndef IMAGEN_H
#define IMAGEN_H

#include <stdio.h>

#include "filtro.h"

typedef struct imagen imagen_t;                                                                                     // TDA imagen, contiene una imagen de un alto y ancho especifico.

imagen_t *imagen_leer_PPM(FILE *f);                                                                                 // Carga un archivo de formato PPM previamente cargado en un FILE *. En caso de falla devuelve NULL.
imagen_t *imagen_leer_BMP(FILE *f);                                                                                 // Carga un archivo de formato BMP previamente cargado en un FILE *. En caso de falla devuelve NULL.
imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto);                 // Crea otro TDA imagen con el recorte del TDA imagen pasado de ancho x alto, donde el recorte empieza en el punto (x0,y0). En caso de que el recorte no sea posible o falle devuelve NULL.
imagen_t *imagen_clonar(const imagen_t *imagen);                                                                    // Crea otro TDA imagen con una copia del TDA imagen pasado. En caso de falla devuelve NULL.
void imagen_espejar_horizontalmente(imagen_t *imagen);                                                              // Espeja horizontalmente el TDA imagen pasado.
void imagen_espejar_verticalmente(imagen_t *imagen);                                                                // Espeja verticalmente el TDA imagen pasado.
void imagen_aplicar_filtro(imagen_t *imagen, filtro_t *filtro);                                                     // Aplica el TDA filtro pasado al TDA imagen pasado.                                                                    
void imagen_escribir_PPM(const imagen_t *imagen, FILE *f);                                                          // Escribe el TDA imagen en un archivo PPM previamente cargado en un FILE *.
void imagen_escribir_BMP(const imagen_t *imagen, FILE *f);                                                          // Escribe el TDA imagen en un archivo BMP previamente cargado en un FILE *.
void imagen_destruir(imagen_t *imagen);                                                                             // Destruye el TDA imagen.

#endif