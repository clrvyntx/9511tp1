#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pixel.h"
#include "endianness.h"

#include "imagen.h"

#define TIPO "P3"
#define LARGO_TIPO 10
#define MAXIMO 255

#define LARGO_ENCABEZADO 2

struct imagen {
    pixel_t **lienzo;
    size_t ancho, alto;
};

static int leer_entero(FILE *f){
    
    char numeros[1000];
    int c;

    while((c=fgetc(f)) != EOF){

        size_t i = 0;  

        if(c == '#'){
            while((c=fgetc(f)) != '\n'){
                continue;
            }
            continue;
        }

        if(isspace(c)){
            continue;
        }

        while(!isspace(c)){
            numeros[i++]=c;
            c=fgetc(f);
            if(c == EOF){
                break;
            }
        }

        numeros[i]='\0';
        return atoi(numeros);
    }

    return -1;
}

void imagen_destruir(imagen_t *imagen){
    for(size_t i = 0; i < imagen->alto; i++){
        free(imagen->lienzo[i]);
    }
    free(imagen->lienzo);
    free(imagen);
}

static imagen_t *_imagen_crear(size_t ancho, size_t alto){
    imagen_t *imagen = malloc(sizeof(imagen_t));
    if(imagen == NULL){
        return NULL;
    }

    imagen->lienzo = malloc(alto*(sizeof(pixel_t*)));
    if(imagen->lienzo == NULL){
        imagen->alto = 0;
        imagen_destruir(imagen);
        return NULL;
    }

    for(size_t i = 0; i < alto; i++){
        imagen->lienzo[i] = malloc(ancho*(sizeof(pixel_t)));
        if(imagen->lienzo[i] == NULL){
            imagen->alto = i;
            imagen_destruir(imagen);
            return NULL;
        }
    }

    imagen->alto=alto;
    imagen->ancho=ancho;

    return imagen;
}

imagen_t *imagen_leer_PPM(FILE *f){
    char tipo[LARGO_TIPO];
    fgets(tipo, LARGO_TIPO, f);
    if(strcmp(tipo, "P3\n")){
        return NULL;
    }

    size_t ancho = leer_entero(f);
    size_t alto = leer_entero(f);
    size_t maximo = leer_entero(f);
    if(maximo != MAXIMO){
        return NULL;
    }

    imagen_t *imagen=_imagen_crear(ancho, alto);
    if(imagen == NULL){
        return NULL;
    }

    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            componente_t r = leer_entero(f);
            componente_t g = leer_entero(f);
            componente_t b = leer_entero(f);
            imagen->lienzo[i][j]=pixel_desde_rgb(r,g,b);
        }
    }

    return imagen;
}

void imagen_escribir_PPM(const imagen_t *imagen, FILE *f){
    fprintf(f,"%s\n",TIPO);
    fprintf(f,"%ld %ld\n",imagen->ancho,imagen->alto);
    fprintf(f,"%d\n",MAXIMO);
    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            pixel_t pixel = imagen->lienzo[i][j];
            fprintf(f,"%d\n%d\n%d\n", pixel_get_rojo(pixel), pixel_get_verde(pixel), pixel_get_azul(pixel));
        }
    }
}

imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto){
    if(x0 >= imagen->ancho || y0 >= imagen->alto){
        return NULL;
    }

    size_t ancho_real = ancho;
    size_t alto_real = alto;

    if(ancho > imagen->ancho-x0){
        ancho_real = imagen->ancho-x0;
    }
    if(alto > imagen->alto-y0){
        alto_real = imagen->alto-y0;
    }
    
    imagen_t *recorte = _imagen_crear(ancho_real, alto_real);
    if(recorte == NULL){
        return NULL;
    }

    for(size_t i = 0; i < recorte->alto; i++){
        for(size_t j = 0; j < recorte->ancho; j++){
            recorte->lienzo[i][j] = imagen->lienzo[y0+i][x0+j];
        }
    }

    return recorte;
}

imagen_t *imagen_clonar(const imagen_t *imagen){
    return imagen_recortar(imagen, 0, 0, imagen->ancho, imagen->alto);
}

void imagen_espejar_horizontalmente(imagen_t *imagen){
    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho/2; j++){
            pixel_t aux = imagen->lienzo[i][j];
            imagen->lienzo[i][j]=imagen->lienzo[i][(imagen->ancho)-1-j];
            imagen->lienzo[i][(imagen->ancho)-1-j]=aux;
        }
    }
}

void imagen_espejar_verticalmente(imagen_t *imagen){
    for(size_t i = 0; i < imagen->alto/2; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            pixel_t aux = imagen->lienzo[i][j];
            imagen->lienzo[i][j]=imagen->lienzo[(imagen->alto)-1-i][j];
            imagen->lienzo[(imagen->alto)-1-i][j]=aux;
        }
    }
}

void imagen_escribir_BMP(const imagen_t *imagen, FILE *f){

    char encabezado[LARGO_ENCABEZADO]= {'B','M'};
    size_t offset_scanline = (imagen->ancho * 3) % 4;
    if(offset_scanline != 0){
        offset_scanline = 4 - offset_scanline;
    }
    int32_t size = 14 + 40 + imagen->alto * ((imagen->ancho * 3) + offset_scanline);

    fwrite(encabezado,1,LARGO_ENCABEZADO,f);
    escribir_int32_little_endian(f,size);
    escribir_int16_little_endian(f,0);
    escribir_int16_little_endian(f,0);
    escribir_int32_little_endian(f,54);
    escribir_int32_little_endian(f,40);
    escribir_int32_little_endian(f,imagen->ancho);
    escribir_int32_little_endian(f,-imagen->alto);
    escribir_int16_little_endian(f,1);
    escribir_int16_little_endian(f,24);
    escribir_int32_little_endian(f,0);
    escribir_int32_little_endian(f,0);
    escribir_int32_little_endian(f,0);
    escribir_int32_little_endian(f,0);
    escribir_int32_little_endian(f,0);
    escribir_int32_little_endian(f,0);

    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            componente_t bgr[3] = {pixel_get_azul(imagen->lienzo[i][j]), pixel_get_verde(imagen->lienzo[i][j]), pixel_get_rojo(imagen->lienzo[i][j])};
            fwrite(bgr,1,3,f);
        }
        if(offset_scanline != 0){
            uint8_t n = 0;
            for(size_t i = 0; i < offset_scanline; i++){
                fwrite(&n,1,1,f);
            }
        }
    }
}

imagen_t *imagen_leer_BMP(FILE *f){
    char encabezado[LARGO_ENCABEZADO];
    fread(encabezado,1,LARGO_ENCABEZADO,f);
    leer_int32_little_endian(f);
    leer_int16_little_endian(f);
    leer_int16_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);
    size_t ancho = leer_int32_little_endian(f);
    int alto = leer_int32_little_endian(f);
    leer_int16_little_endian(f);
    leer_int16_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);
    leer_int32_little_endian(f);

    size_t valor_absoluto_alto = abs(alto);
    size_t offset_scanline = (ancho * 3) % 4;
    if(offset_scanline != 0){
        offset_scanline = 4 - offset_scanline;
    }

    imagen_t *imagen=_imagen_crear(ancho,valor_absoluto_alto);
    if(imagen == NULL){
        return NULL;
    }

    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            componente_t bgr[3] = {0};
            fread(bgr,1,3,f);
            imagen->lienzo[i][j]=pixel_desde_rgb(bgr[2],bgr[1],bgr[0]);
        }
        if(offset_scanline != 0){
            for(size_t i = 0; i < offset_scanline; i++){
                uint8_t n = 0;
                fread(&n,1,1,f);
            }
        }
    }

    if(alto > 0){
        imagen_espejar_verticalmente(imagen);
    }   

    return imagen;
}

void imagen_aplicar_filtro(imagen_t *imagen, filtro_t *filtro){
    for(size_t i = 0; i < imagen->alto; i++){
        for(size_t j = 0; j < imagen->ancho; j++){
            imagen->lienzo[i][j] = aplicar_filtro_en_pixel(imagen->lienzo[i][j],filtro);
        }
    }
}