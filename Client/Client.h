/*
    Client.h

    Raccolta di tutte le funzioni, librerie e macro che saranno usati dal client
*/
#pragma once 

#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_CLIENT 100
#define ID_LEN 9
#define MAX_LEN 201
#define END_MESSAGE nullptr
#define INDIRIZZO_SERVER "127.0.0.1"
#define PORTA_SERVER 4343

// codici controllo errori
#define OK 0
#define NOTOK -1
#define NOTFIND -2

#define DEBUG true

typedef struct user user;
typedef struct msg msg;

typedef enum typFlux typFlux;
typedef enum typSimpleMsg typSimpleMsg;

enum typFlux{
    FRIE_Req,
    FRIE_A,
    FRIE_R,
    MSG,
    FLOO,
};

enum typSimpleMsg{
    WELCO,
    GOBYE,
    FRIE_OK,
    FRIE_NOTOK,
    MESS_OK,
    MESS_NOTOK,
    FLOO_OK,
    ACKRF,
    NOCON,
};

struct msg{
    char id[ID_LEN];
    char value[MAX_LEN]; // 200 caratteri + \0
    typFlux typeMSG;     // gestire il tipo.
    msg *next;
};

struct user{
    char id[ID_LEN];
    struct sockaddr_in add; // ip + porta udp
    unsigned int pw;      // da controllare che sia tra 0 e 65535

    unsigned int nMsg;
    msg *listMsg;

    unsigned int nFri;
    char listFri[MAX_CLIENT - 1][ID_LEN]; // dato che il server ha un massimo di 100 user,
                                          // può avere un massimo 99 amici
};

//-------------------------------TUTTO QUELLO SOPRA METTERE IN LIB COMUNE----------------------------------------------------
//-------------------------------TUTTO QUELLO SOTTO SPECIFICO CLIENT---------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

// Gestisce l'avvio del client.
// sono presenti due socket uno per la comunicazione con il serve tcp, e uno per la gestione dei flussi UDP
// ritorna OK o NOTOK
int startup();

// Effettua la registrazione di un nuovo client al server
// ritorn OK o NOTOK
int newClient();

// Gestisce spegnimento del client, il parametro desc è un vettore di grandezza 2 con in pos 0 il descrittore del socket udo
//e in pos 1 il descrittore del socket tcp. Dato che non è possibile sapere se i socket da chiudere sarà solo il socket tcp, solo udp o entrambi 
//sarà necessario controllare che non siano -1
void client_shutdown(int* desc);

//cose da aggiungere
//Se il client non è registrato dovrà registrarsi, se già registrato procederà ad accedere (prevedere salvataggio credenziali facoltativo senza criptazione per semplicità)
// o chiederndo password o solo username (piccoli cookies?).
//nello specifico all'avvio viene controllato il flusso di messaggi in ingresso per controllare novità.