/*
    Client.h

    Raccolta di tutte le funzioni, librerie e macro che saranno usati dal client
*/
#pragma once

#include "src/utility.h"
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

typedef struct user user;

#define PORTA_UDP_CLIENT 15123  //porta udp ricezione client

// stutture client--------------------------
struct user
{
    char id[ID_LEN];    

    unsigned int mdp; // da controllare che sia tra 0 e 65535

    unsigned int port;
};

// Gestisce l'avvio del client.
// sono presenti due socket uno per la comunicazione con il serve tcp, e uno per la gestione dei flussi UDP
// ritorna OK o NOTOK
int startup();

// Effettua la registrazione di un nuovo client al server
// ritorn OK o NOTOK
int newClient();

// Gestisce spegnimento del client, il parametro desc è un vettore di grandezza 2 con in pos 0 il descrittore del socket udo
// e in pos 1 il descrittore del socket tcp. Dato che non è possibile sapere se i socket da chiudere sarà solo il socket tcp, solo udp o entrambi
// sarà necessario controllare che non siano -1
void client_shutdown();

// cose da aggiungere
// Se il client non è registrato dovrà registrarsi, se già registrato procederà ad accedere (prevedere salvataggio credenziali facoltativo senza criptazione per semplicità)
//  o chiederndo password o solo username (piccoli cookies?).
// nello specifico all'avvio viene controllato il flusso di messaggi in ingresso per controllare novità.