#include <string.h>
#include <math.h>

#include "pixel.h"

#define MASCARA_ROJO 0xFF0000
#define MASCARA_VERDE 0xFF00
#define MASCARA_AZUL 0xFF
#define SHIFT_VERDE 8
#define SHIFT_ROJO 16

typedef enum colores{
    BLACK, WHITE, RED, LIME, BLUE, YELLOW, CYAN, MAGENTA, SILVER, GRAY, MAROON, OLIVE, GREEN, PURPLE, TEAL, NAVY
} colores_t;

typedef struct atributos_colores{
    char *nombre;
    pixel_t rgb;
} atributos_colores_t;

const atributos_colores_t atributos_color[] = {
    [BLACK] = {"black", 0x000000},
    [WHITE] = {"white", 0xFFFFFF},
    [RED] = {"red", 0xFF0000},
    [LIME] = {"lime", 0x00FF00},
    [BLUE] = {"blue", 0x0000FF},
    [YELLOW] = {"yellow", 0xFFFF00},
    [CYAN] = {"cyan", 0x00FFFF},
    [MAGENTA] = {"magenta", 0xFF00FF},
    [SILVER] = {"silver", 0xC0C0C0},
    [GRAY] = {"gray", 0x808080},
    [MAROON] = {"maroon", 0x800000},
    [OLIVE] = {"olive", 0x808000},
    [GREEN] = {"green", 0x008000},
    [PURPLE] = {"purple", 0xC00080},
    [TEAL] = {"teal", 0x008080},
    [NAVY] = {"navy", 0x000080},
};

pixel_t pixel_desde_rgb(componente_t rojo, componente_t verde, componente_t azul){
    return (rojo << SHIFT_ROJO) | (verde << SHIFT_VERDE) | azul;
}

componente_t pixel_get_rojo(pixel_t p){
    return ((p & MASCARA_ROJO) >> SHIFT_ROJO);
}

componente_t pixel_get_verde(pixel_t p){
    return ((p & MASCARA_VERDE) >> SHIFT_VERDE);
}

componente_t pixel_get_azul(pixel_t p){
    return (p & MASCARA_AZUL);
}

void pixel_a_hsv(pixel_t p, short *h, float *s, float *v) {
    float rp = pixel_get_rojo(p) / 255.0;
    float vp = pixel_get_verde(p) / 255.0;
    float ap = pixel_get_azul(p) / 255.0;

    float cmax = rp > vp ? rp : vp;
    cmax = cmax > ap ? cmax : ap;

    float cmin = rp < vp ? rp : vp;
    cmin = cmin < ap ? cmin : ap;

    float delta = cmax - cmin;

    if(delta == 0)
        *h = 0;
    else if(cmax == rp)
        *h = (int)(60 * (vp - ap) / delta + 360.5) % 360;
    else if(cmax == vp)
        *h = (int)(60 * (ap - rp) / delta + 120.5) % 360;
    else
        *h = (int)(60 * (rp - vp) / delta + 240.5) % 360;

    *s = cmax == 0 ? 0 : (delta / cmax);
    *v = cmax;
}

pixel_t pixel_desde_hsv(short h, float s, float v) {
    float c = s * v;
    float x = c * (1 - fabs(fmodf(h / 60.0, 2) - 1));
    float m = v - c;

    float r = 0, v_ = 0, a = 0;

    if(h < 60)
        r = c, v_ = x;
    else if(h < 120)
        v_ = c, r = x;
    else if(h < 180)
        v_ = c, a = x;
    else if(h < 240)
        a = c, v_ = x;
    else if(h < 300)
        a = c, r = x;
    else
        r = c, a = x;

    return pixel_desde_rgb((r + m) * 255, (v_ + m) * 255, (a + m) * 255);
}

bool pixel_desde_nombre(pixel_t *p, const char *nombre){
    size_t cantidad = sizeof(atributos_color)/sizeof(atributos_color[0]);
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(nombre,atributos_color[i].nombre)){
            *p = atributos_color[i].rgb;
            return true;
        }
    }
    return false;
}