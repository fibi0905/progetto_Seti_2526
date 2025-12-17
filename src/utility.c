#include "utility.h"
#include <stdio.h>

unsigned int litEndianTOusingedInt(unsigned char mdpBYTE [2]){
    // Converte la password da little-endian a unsigned int
    return mdpBYTE[0] | (mdpBYTE[1] << 8);
    /*
        << fa uno shift a sinistra di 8 bit
        poi mette in or con il byte meno significativo 
        
    */
}


void usingedIntTOlitEndian(unsigned int value, char hexString[3]) {
    // Limita il valore a 0-255 (un byte)
    unsigned char byte = (unsigned char)(value & 0xFF);
    
    // Converte in stringa esadecimale con 2 cifre
    sprintf(hexString, "%02X", byte);
    // hexString conterrà "00" per 0, "01" per 1, "0A" per 10, "FF" per 255
}