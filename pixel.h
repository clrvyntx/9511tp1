#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t componente_t;                                                                   // TDA componente, representa una de las 3 componentes RGB.
typedef uint32_t pixel_t;                                                                       // TDA pixel, representa un pixel RGB.

pixel_t pixel_desde_rgb(componente_t rojo, componente_t verde, componente_t azul);              // Crea un TDA pixel a traves de 3 componentes RGB.
pixel_t pixel_desde_hsv(short h, float s, float v);                                             // Crea un TDA pixel a traves de 3 componentes HSV.
bool pixel_desde_nombre(pixel_t *p, const char *nombre);                                        // Recibe un TDA pixel pasado por referencia e intenta cargarle un color a partir de una cadena con su nombre. En caso de que el color no sea valido devuelve false.
componente_t pixel_get_rojo(pixel_t p);                                                         // Devuelve el TDA componente correspondiente a la componente R del TDA pixel.
componente_t pixel_get_verde(pixel_t p);                                                        // Devuelve el TDA componente correspondiente a la componente G del TDA pixel.
componente_t pixel_get_azul(pixel_t p);                                                         // Devuelve el TDA componente correspondiente a la componente B del TDA pixel.
void pixel_a_hsv(pixel_t p, short *h, float *s, float *v);                                      // Carga el TDA pixel en 3 valores h,s,v pasados por referencia.

#endif