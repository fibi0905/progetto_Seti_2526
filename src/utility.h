#pragma once

//librerie comuni
#include <netinet/in.h>
#include <stdlib.h>

// codici controllo errori
#define OK 0
#define NOTOK -1
#define NOTFIND -2

#define ID_LEN 9

#define DEBUG true  //controllo modalita verbose

//sarebbe 218 il massimo, visto che il messaggio più lungo 
//è l'invio di un messaggio dato che:
//MESS? -> 5byte + id-> 8byte + mess-> 200byte + "+++" ->3byte + "_ _" ->2byte  = per un totale di 218 
#define MAX_TCP_MESAGGE 256 
#define MAX_LEN 201

#define SERVER_PORT 8080
#define INDIRIZZO_SERVER "127.0.0.1"
