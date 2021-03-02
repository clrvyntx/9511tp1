#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "filtro.h"

#include "argumentos.h"

static bool cargar_input(argumentos_t *, int, char **, size_t *);
static bool cargar_output(argumentos_t *, int, char **, size_t *);
static bool cargar_filtro(argumentos_t *, int, char **, size_t *);
static bool cargar_crop(argumentos_t *, int, char **, size_t *);
static bool cargar_espejado_horizontal(argumentos_t *, int, char **, size_t *);
static bool cargar_espejado_vertical(argumentos_t *, int, char **, size_t *);

typedef enum extension{
    PPM , BMP
} extension_t;

typedef enum argumento{
    INPUT, OUTPUT, FILTER, CROP, HORIZONTAL, VERTICAL
} argumento_t;

typedef bool (*funciones_argumentos_t)(argumentos_t *, int, char **, size_t *);
typedef imagen_t *(*funciones_lectura_archivos_t)(FILE *);
typedef void (*funciones_escritura_archivos_t)(const imagen_t *, FILE*);

typedef struct atributos_argumentos{
    char *nombre;
    funciones_argumentos_t funcion;
} atributos_argumentos_t;

typedef struct atributos_extensiones{
    char *extension;
    char *modo_lectura;
    char *modo_escritura;
    funciones_lectura_archivos_t funcion_lectura;
    funciones_escritura_archivos_t funcion_escritura;
} atributos_extensiones_t;

const atributos_extensiones_t atributos_extension[] = {
    [PPM] = {".ppm","r","w", imagen_leer_PPM, imagen_escribir_PPM},
    [BMP] = {".bmp","rb","wb", imagen_leer_BMP, imagen_escribir_BMP},
};

const atributos_argumentos_t atributos_argumentos[] = {
    [INPUT] = {"-input", cargar_input},
    [OUTPUT] = {"-output", cargar_output},
    [FILTER] = {"-filter", cargar_filtro},
    [CROP] = {"-crop", cargar_crop},
    [HORIZONTAL] = {"-horizontal", cargar_espejado_horizontal},
    [VERTICAL] = {"-vertical", cargar_espejado_vertical},
};

typedef struct archivo{
    char *archivo;
    char *modo_lectura;
    char *modo_escritura;
    extension_t extension;
} archivo_t;

typedef struct recorte{
    size_t ancho;
    size_t alto;
    size_t x;
    size_t y;
} parametros_recorte_t;

typedef struct operaciones{
    parametros_recorte_t *parametros_recorte;
    filtro_t *parametros_filtro;
    bool es_recorte;
    bool es_filtro;
    bool es_horizontal;
    bool es_vertical;
} operaciones_t;

struct argumentos{
    archivo_t *entrada;
    archivo_t *salida;
    operaciones_t *operaciones;
    size_t cantidad_operaciones;
    bool cargado;
};

static char *crear_cadena_dinamica(const char *cadena){
    size_t len = strlen(cadena);
    char *cadena_dinamica = malloc((len+1)*sizeof(char));
    if(cadena == NULL){
        return NULL;
    }
    for(size_t i = 0; i < len; i++){
        cadena_dinamica[i] = cadena[i];
    }
    cadena_dinamica[len] = '\0';
    return cadena_dinamica;
}

static bool extension_desde_nombre(char *archivo, extension_t *extension_archivo){
    if(archivo[0] == '.'){
        return false;
    }
    char *extension = strrchr(archivo,'.');
    if(extension == NULL){
        return false;
    }
    size_t cantidad = sizeof(atributos_extension)/sizeof(atributos_extension[0]);
    for(size_t i = 0; i < cantidad; i++){
        if(!strcmp(extension,atributos_extension[i].extension)){
            *extension_archivo = i;
            return true;
        }
    }
    return false;
}

static void destruir_operaciones(argumentos_t *argumentos){
    for(size_t i = 0; i < argumentos->cantidad_operaciones; i++){
        free(argumentos->operaciones[i].parametros_recorte);
        destruir_filtro(argumentos->operaciones[i].parametros_filtro);
    }
    free(argumentos->operaciones);
}

static void settear_vector_operaciones_vacio(operaciones_t *operaciones, size_t n){
    operaciones[n].parametros_filtro = NULL;
    operaciones[n].parametros_recorte = NULL;
    operaciones[n].es_filtro = false;
    operaciones[n].es_recorte = false;
    operaciones[n].es_horizontal = false;
    operaciones[n].es_vertical = false;
}

static parametros_recorte_t *leer_recorte(char *entrada){
    parametros_recorte_t *recorte = malloc(sizeof(parametros_recorte_t));
    if(recorte == NULL){
        return NULL;
    }

    char numeros[1000];
    size_t contador_caracter = 0;
    size_t contador_parametro = 0;
    size_t len = strlen(entrada);
    size_t parametros[4];
    int cortes[4] = {'x','+','+','\0'};

    while(contador_caracter < len){
        size_t i = 0;

        while(entrada[contador_caracter] != cortes[contador_parametro]){
            if(!isdigit(entrada[contador_caracter])){
                free(recorte);
                return NULL;
            }
            numeros[i] = entrada[contador_caracter];
            i++;
            contador_caracter++;
        }

        if(i == 0){
            free(recorte);
            return NULL;
        }

        numeros[i] = '\0';
        parametros[contador_parametro] = atoi(numeros);
        contador_parametro++;
        contador_caracter++;
    }

    if(contador_parametro != 4){
        free(recorte);
        return NULL;
    }

    recorte->ancho = parametros[0];
    recorte->alto = parametros[1];
    recorte->x = parametros[2];
    recorte->y = parametros[3];

    return recorte;
}

argumentos_t *crear_vector_argumentos_vacio(){
    argumentos_t *argumentos = malloc(sizeof(argumentos_t));
    if(argumentos == NULL){
        return NULL;
    }

    argumentos->entrada = NULL;
    argumentos->salida = NULL;
    argumentos->operaciones = NULL;
    argumentos->cantidad_operaciones = 0;
    argumentos->cargado = false;

    return argumentos;
}

void destruir_argumentos(argumentos_t *argumentos){
    if(argumentos->entrada != NULL){
        free(argumentos->entrada->archivo);
        free(argumentos->entrada->modo_lectura);
        free(argumentos->entrada->modo_escritura);
        free(argumentos->entrada);
    }
    if(argumentos->salida != NULL){
        free(argumentos->salida->archivo);
        free(argumentos->salida->modo_lectura);
        free(argumentos->salida->modo_escritura);
        free(argumentos->salida);
    }
    if(argumentos->operaciones != NULL){
        destruir_operaciones(argumentos);
    }
    free(argumentos);
}

bool leer_argumentos(argumentos_t *argumentos, int argc, char *argv[]){
    size_t contador = 1;
    size_t cantidad = sizeof(atributos_argumentos)/sizeof(atributos_argumentos[0]);
    while(contador < argc){
        bool valido = false;
        for(size_t i = 0; i < cantidad; i++){
            if(!strcmp(argv[contador],atributos_argumentos[i].nombre)){
                if(!atributos_argumentos[i].funcion(argumentos,argc,argv,&contador)){
                    return false;
                } else {
                    valido = true;
                    break;
                }
            }
        }
        if(!valido){
            return false;
        }
    }
    argumentos->cargado = true;
    return true;
}

static bool cargar_input(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    if(*contador == argc-1){
        return false;
    }

    if(argumentos->entrada != NULL){
        return false;
    }

    archivo_t *aux = malloc(sizeof(archivo_t));
    if(aux == NULL){
        return false;
    }

    (*contador)++;

    extension_t extension;
    if(!extension_desde_nombre(argv[*contador],&extension)){
        free(aux);
        return false;
    }

    char *archivo = crear_cadena_dinamica(argv[*contador]);
    if(archivo == NULL){
        free(aux);
        return false;
    }

    char *lectura = crear_cadena_dinamica(atributos_extension[extension].modo_lectura);
    if(lectura == NULL){
        free(archivo);
        free(aux);
        return false;
    }

    char *escritura = crear_cadena_dinamica(atributos_extension[extension].modo_escritura);
    if(escritura == NULL){
        free(archivo);
        free(lectura);
        free(aux);
        return false;
    }

    argumentos->entrada = aux;
    argumentos->entrada->archivo = archivo;
    argumentos->entrada->modo_lectura = lectura;
    argumentos->entrada->modo_escritura = escritura;
    argumentos->entrada->extension = extension;
    (*contador)++;
    return true;
}

static bool cargar_output(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    if(*contador == argc-1){
        return false;
    }

    if(argumentos->salida != NULL){
        return false;
    }

    archivo_t *aux = malloc(sizeof(archivo_t));
    if(aux == NULL){
        return false;
    }

    (*contador)++;

    extension_t extension;
    if(!extension_desde_nombre(argv[*contador],&extension)){
        free(aux);
        return false;
    }

    char *archivo = crear_cadena_dinamica(argv[*contador]);
    if(archivo == NULL){
        free(aux);
        return false;
    }

    char *lectura = crear_cadena_dinamica(atributos_extension[extension].modo_lectura);
    if(lectura == NULL){
        free(archivo);
        free(aux);
        return false;
    }

    char *escritura = crear_cadena_dinamica(atributos_extension[extension].modo_escritura);
    if(escritura == NULL){
        free(archivo);
        free(lectura);
        free(aux);
        return false;
    }

    argumentos->salida = aux;
    argumentos->salida->archivo = archivo;
    argumentos->salida->modo_lectura = lectura;
    argumentos->salida->modo_escritura = escritura;
    argumentos->salida->extension = extension;
    (*contador)++;
    return true;
}

static bool cargar_espejado_horizontal(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    operaciones_t *aux = realloc(argumentos->operaciones,(argumentos->cantidad_operaciones+1)*sizeof(operaciones_t));
    if(aux == NULL){
        return false;
    }

    settear_vector_operaciones_vacio(aux,argumentos->cantidad_operaciones);
    argumentos->operaciones = aux;
    argumentos->operaciones[argumentos->cantidad_operaciones].es_horizontal = true;
    argumentos->cantidad_operaciones++;
    (*contador)++;
    return true;
}

static bool cargar_espejado_vertical(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    operaciones_t *aux = realloc(argumentos->operaciones,(argumentos->cantidad_operaciones+1)*sizeof(operaciones_t));
    if(aux == NULL){
        return false;
    }

    settear_vector_operaciones_vacio(aux,argumentos->cantidad_operaciones);
    argumentos->operaciones = aux;
    argumentos->operaciones[argumentos->cantidad_operaciones].es_vertical = true;
    argumentos->cantidad_operaciones++;
    (*contador)++;
    return true;
}

static bool cargar_crop(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    if(*contador == argc-1){
        return false;
    }

    (*contador)++;
    parametros_recorte_t *recorte = leer_recorte(argv[*contador]);
    if(recorte == NULL){
        return false;
    }

    operaciones_t *aux = realloc(argumentos->operaciones,(argumentos->cantidad_operaciones+1)*sizeof(operaciones_t));
    if(aux == NULL){
        return false;
    }

    settear_vector_operaciones_vacio(aux,argumentos->cantidad_operaciones);
    argumentos->operaciones = aux;
    argumentos->operaciones[argumentos->cantidad_operaciones].parametros_recorte = recorte;
    argumentos->operaciones[argumentos->cantidad_operaciones].es_recorte = true;
    argumentos->cantidad_operaciones++;
    (*contador)++;
    return true;
}

static bool cargar_filtro(argumentos_t *argumentos, int argc, char *argv[], size_t *contador){
    if(*contador == argc-1){
        return false;
    }

    (*contador)++;

    filtro_t *filtro = crear_filtro(argv[*contador]);
    if(filtro == NULL){
        return false;
    }

    if(numero_de_parametros(filtro) == 1){
        if(*contador == argc-1){
            destruir_filtro(filtro);
            return false;
        }

        (*contador)++;
        if(!cargar_parametro_a_filtro(filtro,argv[*contador])){
            destruir_filtro(filtro);
            return false;
        }
    }

    operaciones_t *aux = realloc(argumentos->operaciones,(argumentos->cantidad_operaciones+1)*sizeof(operaciones_t));
    if(aux == NULL){
        destruir_filtro(filtro);
        return false;
    }

    settear_vector_operaciones_vacio(aux,argumentos->cantidad_operaciones);
    argumentos->operaciones = aux;
    argumentos->operaciones[argumentos->cantidad_operaciones].parametros_filtro = filtro;
    argumentos->operaciones[argumentos->cantidad_operaciones].es_filtro = true;
    argumentos->cantidad_operaciones++;
    (*contador)++;
    return true;
}

bool es_recorte(argumentos_t *argumentos, size_t n){
    return ((argumentos->cargado != false) && (argumentos->operaciones[n].es_recorte));
}

bool es_filtro(argumentos_t *argumentos, size_t n){
    return ((argumentos->cargado != false) && (argumentos->operaciones[n].es_filtro));
}

bool es_espejo_horizontal(argumentos_t *argumentos, size_t n){
    return ((argumentos->cargado != false) && (argumentos->operaciones[n].es_horizontal));
}

bool es_espejo_vertical(argumentos_t *argumentos, size_t n){
    return ((argumentos->cargado != false) && (argumentos->operaciones[n].es_vertical));
}

size_t cantidad_operaciones(argumentos_t *argumentos){
    return argumentos->cantidad_operaciones;
}

imagen_t *cargar_imagen_desde_argumentos(argumentos_t *argumentos){
    if(argumentos->cargado == false){
        return NULL;
    }
    if(argumentos->entrada == NULL){
        return imagen_leer_PPM(stdin);
    } else {
        FILE *entrada = fopen(argumentos->entrada->archivo,argumentos->entrada->modo_lectura);
        if(entrada == NULL){
            return NULL;
        }
        imagen_t *aux = atributos_extension[argumentos->entrada->extension].funcion_lectura(entrada);
        fclose(entrada);
        return aux;
    }
}

bool escribir_imagen_desde_argumentos(argumentos_t *argumentos, imagen_t *imagen){
    if(argumentos->cargado == false){
        return false;
    }
    if(argumentos->salida == NULL){
        imagen_escribir_PPM(imagen,stdout);
        return true;
    } else {
        FILE *salida = fopen(argumentos->salida->archivo,argumentos->salida->modo_escritura);
        if(salida == NULL){
            return false;
        }
        atributos_extension[argumentos->salida->extension].funcion_escritura(imagen,salida);
        fclose(salida);
        return true;
    }
}

imagen_t *realizar_recorte(argumentos_t *argumentos, size_t n, imagen_t *imagen){
    if((argumentos->cargado == false) || (!argumentos->operaciones[n].es_recorte)){
        return NULL;
    }
    imagen_t *aux = imagen_recortar(imagen, argumentos->operaciones[n].parametros_recorte->x, argumentos->operaciones[n].parametros_recorte->y, argumentos->operaciones[n].parametros_recorte->ancho, argumentos->operaciones[n].parametros_recorte->alto);
    return aux;
}

bool aplicar_filtro(argumentos_t *argumentos, size_t n, imagen_t *imagen){
    if((argumentos->cargado == false) || (!argumentos->operaciones[n].es_filtro)){
        return false;
    }
    imagen_aplicar_filtro(imagen,argumentos->operaciones[n].parametros_filtro);
    return true;
}