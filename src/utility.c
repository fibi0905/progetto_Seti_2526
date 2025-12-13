#include "utility.h"


unsigned int litEndianTOusingedInt(unsigned char mdpBYTE [2]){
    // Converte la password da little-endian a unsigned int
    return mdpBYTE[0] | (mdpBYTE[1] << 8);
    /*
        << fa uno shift a sinistra di 8 bit
        poi mette in or con il byte meno significativo 
        
    */
}


void usingedIntTOlitEndian(unsigned int value, unsigned char array[2]) {
    array[0] = (char)(value & 0xFF);        // byte meno significativo
    array[1] = (char)((value >> 8) & 0xFF); // byte più significativo
}