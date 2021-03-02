#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "filtro.h"

typedef enum filtros{
    FILTRO_INVERTIR, FILTRO_SATURAR, FILTRO_CAMBIAR_GAMA, FILTRO_CAMBIAR_BRILLO, FILTRO_CAMBIAR_CONTRASTE, FILTRO_MEZCLAR, FILTRO_SEPIA, FILTRO_MONOCROMO, FILTRO_TOASTER, FILTRO_VALENCIA
} filtros_t;

typedef pixel_t (*funciones_filtros_t)(pixel_t, int);

typedef struct atributos_filtros{
    char *nombre;
    int parametros;
    funciones_filtros_t funcion;
} atributos_filtros_t;

static pixel_t filtro_invertir(pixel_t , int);
static pixel_t filtro_sepia(pixel_t , int);
static pixel_t filtro_toaster(pixel_t , int);
static pixel_t filtro_valencia(pixel_t , int);
static pixel_t filtro_saturar(pixel_t , int);
static pixel_t filtro_cambiar_gama(pixel_t , int);
static pixel_t filtro_cambiar_brillo(pixel_t , int);
static pixel_t filtro_cambiar_contraste(pixel_t , int);
static pixel_t filtro_mezclar(pixel_t , int);
static pixel_t filtro_monocromo(pixel_t , int);

const atributos_filtros_t atributos_filtro[] = {
    [FILTRO_INVERTIR] = {"invertir", 0, filtro_invertir},
    [FILTRO_SATURAR] = {"saturar", 1, filtro_saturar},
    [FILTRO_CAMBIAR_GAMA] = {"gama", 1, filtro_cambiar_gama},
    [FILTRO_CAMBIAR_BRILLO] = {"brillo", 1, filtro_cambiar_brillo},
    [FILTRO_CAMBIAR_CONTRASTE] = {"contraste", 1, filtro_cambiar_contraste},
    [FILTRO_MEZCLAR] = {"mezclar", 1, filtro_mezclar},
    [FILTRO_SEPIA] = {"sepia", 0, filtro_sepia},
    [FILTRO_MONOCROMO] = {"monocromo", 1, filtro_monocromo},
    [FILTRO_TOASTER] = {"toaster", 0, filtro_toaster},
    [FILTRO_VALENCIA] = {"valencia", 0, filtro_valencia},
};

struct filtro{
    filtros_t filtro;
    int cantidad_parametros;
    int parametro;
};

static pixel_t filtro_invertir(pixel_t p, int _) {
    return pixel_desde_rgb(255 - pixel_get_rojo(p), 255 - pixel_get_verde(p), 255 - pixel_get_azul(p));
}

static pixel_t filtro_saturar(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    s *= (1 + porcentaje / 100.0);

    if(s < 0)
        s = 0;
    else if(s > 1)
        s = 1;

    return pixel_desde_hsv(h, s, v);
}

static pixel_t filtro_cambiar_gama(pixel_t p, int desplazamiento) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    h = (h + desplazamiento) % 360;

    return pixel_desde_hsv(h, s, v);
}

static pixel_t filtro_cambiar_brillo(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v += porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

static pixel_t filtro_cambiar_contraste(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v *= 1 + porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

static pixel_t filtro_mezclar(pixel_t p, int color) {
    pixel_t c = color;

    return pixel_desde_rgb(
        (pixel_get_rojo(p) + pixel_get_rojo(c)) / 2,
        (pixel_get_verde(p) + pixel_get_verde(c)) / 2,
        (pixel_get_azul(p) + pixel_get_azul(c)) / 2);
}

static pixel_t filtro_sepia(pixel_t p, int _) {
    componente_t r = pixel_get_rojo(p);
    componente_t v = pixel_get_verde(p);
    componente_t a = pixel_get_azul(p);

    int rn = r * 0.393 + v * 0.769 + a * 0.189;
    int vn = r * 0.349 + v * 0.686 + a * 0.168;
    int an = r * 0.272 + v * 0.534 + a * 0.131;

    if(rn > 255) rn = 255;
    if(vn > 255) vn = 255;
    if(an > 255) an = 255;

    return pixel_desde_rgb(rn, vn, an);
}

static pixel_t filtro_monocromo(pixel_t p, int porcentaje_umbral) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    return (v > porcentaje_umbral / 100.) ? pixel_desde_rgb(255, 255, 255) : pixel_desde_rgb(0, 0, 0);
}

static pixel_t filtro_toaster(pixel_t p, int _) {
    p = filtro_mezclar(p, pixel_desde_rgb(51, 0, 0));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.5 + 0.2;
    s *= 0.8;
    h = (h + 20) % 360;

    if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

static pixel_t filtro_valencia(pixel_t p, int _) {
    float r = pixel_get_rojo(p) / 100.;
    float ve = pixel_get_verde(p) / 100.;
    float a = pixel_get_azul(p) / 100.;

    r = 0.23 + r - 2 * 0.23 * r;
    ve = 0.01 + ve - 2 * 0.01 * ve;
    a = 0.22 + a - 2 * 0.22 * a;

    if(r > 1) r = 1;
    if(ve > 1) ve = 1;
    if(a > 1) a = 1;

    if(r < 0) r = 0;
    if(ve < 0) ve = 0;
    if(a < 0) a = 0;

    p = filtro_mezclar(p, pixel_desde_rgb(r * 255, ve * 255, a * 255));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.08 - 0.08;
    if(v > 1)
        v = 1;
    if(v < 0)
        v = 0;

    p = pixel_desde_hsv(h, s, v);
    pixel_t c = filtro_sepia(p, 0);

    return pixel_desde_rgb(
        pixel_get_rojo(p) * 0.92 + pixel_get_rojo(c) * 0.08,
        pixel_get_verde(p) * 0.92 + pixel_get_verde(c) * 0.08,
        pixel_get_azul(p) * 0.92 + pixel_get_azul(c) * 0.08);
}

static bool es_entero(const char *input){
    for(size_t i = 0; input[i]; i++){
        if(!isdigit(input[i])){
            if(i != 0){
                return false;
            } else {
                if(input[i] != '-'){
                    return false;
                } else {
                    continue;
                }
            }
        }
    }
    return true;
}

filtro_t *crear_filtro(const char *nombre){
    size_t cantidad = sizeof(atributos_filtro)/sizeof(atributos_filtro[0]);
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(nombre,atributos_filtro[i].nombre)){
            filtro_t *filtro = malloc(sizeof(filtro_t));
            if(filtro == NULL){
                return NULL;
            }
            filtro->filtro = i;
            filtro->cantidad_parametros = atributos_filtro[i].parametros;
            filtro->parametro = 0;
            return filtro;
        }
    }
    return NULL;
}

bool cargar_parametro_a_filtro(filtro_t *filtro, const char *parametro){
    if(filtro->cantidad_parametros == 0){
        return false;
    }
    if(!strcmp(atributos_filtro[filtro->filtro].nombre,"mezclar")){
        pixel_t p;
        if(!pixel_desde_nombre(&p,parametro)){
            return false;
        }
        filtro->parametro = p;
        return true;
    } else {
        if(!es_entero(parametro)){
            return false;
        }
        filtro->parametro = atoi(parametro);
        return true;
    }
}

int numero_de_parametros(filtro_t *filtro){
    return filtro->cantidad_parametros;
}

pixel_t aplicar_filtro_en_pixel(pixel_t p, filtro_t *filtro){
    return atributos_filtro[filtro->filtro].funcion(p,filtro->parametro);
}

void destruir_filtro(filtro_t *filtro){
    free(filtro);
}