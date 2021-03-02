#include "argumentos.h"

int main(int argc, char *argv[]){
    argumentos_t *settings = crear_vector_argumentos_vacio();
    if(settings == NULL){
        fprintf(stderr,"Hubo un error creando el vector de operaciones\n");
        return 1;
    }

    if(!leer_argumentos(settings,argc,argv)){
        fprintf(stderr,"Hay argumentos invalidos, no se puede editar la foto\n");
        destruir_argumentos(settings);
        return 1;
    }

    imagen_t *imagen = cargar_imagen_desde_argumentos(settings);
    if(imagen == NULL){
        fprintf(stderr,"Hubo un error cargando el archivo de entrada, verificar que la entrada sea un archivo valido\n");
        destruir_argumentos(settings);
        return 1;
    }

    size_t operaciones = cantidad_operaciones(settings);
    for(size_t i = 0; i < operaciones; i++){
        if(es_espejo_horizontal(settings,i)){
            imagen_espejar_horizontalmente(imagen);
        }

        if(es_espejo_vertical(settings,i)){
            imagen_espejar_verticalmente(imagen);
        }

        if(es_recorte(settings,i)){
            imagen_t *recorte = realizar_recorte(settings,i,imagen);
            if(recorte == NULL){
                fprintf(stderr,"Hubo un error recortando la imagen, verificar que sea posible el recorte pedido\n");
                imagen_destruir(imagen);
                destruir_argumentos(settings);
                return 1;
            }
            imagen_destruir(imagen);
            imagen = recorte;
        }

        if(es_filtro(settings,i)){
            if(!aplicar_filtro(settings,i,imagen)){
                fprintf(stderr,"Hubo un error aplicando un filtro, verificar que los parametros del filtro sean correctos\n");
                imagen_destruir(imagen);
                destruir_argumentos(settings);
                return 1;
            }
        }
    }

    if(!escribir_imagen_desde_argumentos(settings,imagen)){
        fprintf(stderr,"Hubo un error escribiendo en el archivo de salida, verificar que la salida sea un archivo valido\n");
        imagen_destruir(imagen);
        destruir_argumentos(settings);
        return 1;
    }

    fprintf(stderr,"La imagen fue editada satisfactoriamente\n");
    imagen_destruir(imagen);
    destruir_argumentos(settings);
    return 0;
}