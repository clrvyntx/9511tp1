#include "endianness.h"

#define MASCARA_8 0xFF
#define MASCARA_16 0xFF00
#define MASCARA_24 0xFF0000
#define MASCARA_32 0xFF000000

#define SHIFT_16 8
#define SHIFT_24 16
#define SHIFT_32 24

void escribir_int16_little_endian(FILE *f, int16_t v){
    uint8_t int16[2] = {(v & MASCARA_8) , ((v & MASCARA_16) >> SHIFT_16)};
    fwrite(int16,1,2,f);
}

int16_t leer_int16_little_endian(FILE *f){
    uint8_t bytes[2] = {0};
    fread(bytes,1,2,f);
    return (bytes[0] | bytes[1] << SHIFT_16);
}

void escribir_int32_little_endian(FILE *f, int32_t v){
    uint8_t int32[4] = {(v & MASCARA_8) , ((v & MASCARA_16) >> SHIFT_16), ((v & MASCARA_24) >> SHIFT_24), ((v & MASCARA_32) >> SHIFT_32)};
    fwrite(int32,1,4,f);
}

int32_t leer_int32_little_endian(FILE *f){
    uint8_t bytes[4] = {0};
    fread(bytes,1,4,f);
    return (bytes[0] | bytes[1] << SHIFT_16 | bytes[2] << SHIFT_24 | bytes[3] << SHIFT_32);
}