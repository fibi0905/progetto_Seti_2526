/*
    Client.h

    Raccolta di tutte le funzioni, librerie e macro che saranno usati dal client
*/
#pragma once


//LIBRERIE DEDICATE ---------------------------------------------------------------------------------
#include "../src/utility.h"
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <endian.h>

//TYPEDEF ------------------------------------------------------------------------------------------
typedef struct user user;

//INIZIO DEFINE ------------------------------------------------------------------------------------
#define PORTA_UDP_CLIENT 9999  //porta udp ricezione client
#define DEBUG false // controllo modalita verbose predefinita
#define DIMBUF 256   //dimensione buffer
//credenziali predefinite --------------------------------------------------------------------------
#define PASSWORD_STD 0
#define ID_STD "Ferraro"

// STRUTTURA USER ----------------------------------------------------------------------------------
struct user
{
    char id[ID_LEN];    //max 8 caratteri

    unsigned int mdp;   // da controllare che sia tra 0 e 65535

    unsigned int port;  //porta udp
};

//FUNZIONI ESPOSTE ---------------------------------------------------------------------------------

//inizializza tutte le impostazioni del client; prende come parametri argc (numero parametri) e args(array di stringhe con opzioni)
int initialization(const int, const char**);

// Gestisce l'avvio del client.
// sono presenti due socket uno per la comunicazione con il serve tcp, e uno per la gestione dei flussi UDP
// ritorna OK o NOTOK
int startup();

// Effettua la registrazione di un nuovo client al server
// ritorn OK o NOTOK
int newClient();

//effettua l'accesso con le credenziali salvate se disponibili, altrimenti le richiede all'utente
int login();

//effettua una richiesta d'amicizia ad un altro client, riceve come parametro id del client a cui sarà inviata la richiesta
int friend_request(char*);

// Gestisce spegnimento del client, il parametro desc è un vettore di grandezza 2 con in pos 0 il descrittore del socket udo
// e in pos 1 il descrittore del socket tcp. Dato che non è possibile sapere se i socket da chiudere sarà solo il socket tcp, solo udp o entrambi
// sarà necessario controllare che non siano -1
void client_shutdown();

// setter della variabile globale debug che gestisce modalita verbose
void set_debug(const bool);

// setter della variabile globale che gestisce l'uso delle credenziali predefinite standard
void set_credenzialiDefault(const bool);

//funzione di stampa per debug
void debug(const char*, ...);


//NOTE FINALI -7777777-7-7-7--7-7-7-7--7

// cose da aggiungere
// Se il client non è registrato dovrà registrarsi, se già registrato procederà ad accedere (prevedere salvataggio credenziali facoltativo senza criptazione per semplicità)
//  o chiederndo password o solo username (piccoli cookies?).
// nello specifico all'avvio viene controllato il flusso di messaggi in ingresso per controllare novità.