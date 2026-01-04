
// librerie comuni
#include <netinet/in.h>
#include <stdlib.h>

// codici controllo errori
#define OK 1
#define NOTOK 0
#define NOTFIND -1
#define FRIEND_REQUEST 2
#define ID_LEN 9    //sicuri 9?? \n finale forse non importa


// sarebbe 218 il massimo, visto che il messaggio più lungo
// è l'invio di un messaggio dato che:
// MESS? -> 5byte + id-> 8byte + mess-> 200byte + "+++" ->3byte + "_ _" ->2byte  = per un totale di 218
#define MAX_TCP_MESAGGE 256
#define MAX_LEN 201

#define SERVER_PORT 8080
#define INDIRIZZO_SERVER "0.0.0.0"



//converte da little-endian a unsigned int 
unsigned int litEndianTOusingedInt(unsigned char mdpBYTE [2]);

//converte da usinged int a littel-endian
void usingedIntTOlitEndian(unsigned int value, char hexString[3]);


void add0 ( int n, char *Ris );