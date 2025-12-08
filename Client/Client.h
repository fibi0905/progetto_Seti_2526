/*
    Client.h

    Raccolta di tutte le funzioni, librerie e macro che saranno usati dal client
*/
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_CLIENT 100
#define ID_LEN 9    //forse 8 dato che sono 8 caratteri alfanumerici ed è codificato su 8 byte e non 9 come in questo caso
#define MAX_LEN 201

//mofificati rispetto a fabio
#define OK 0
#define NOTOK -1
#define NOTFIND -2


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
    char id [ID_LEN];
    char  value [MAX_LEN]; //200 caratteri + \0
    typFlux typeMSG; //gestire il tipo.
    msg* next;
};

struct user{
    char id [ID_LEN];
    struct sockaddr_in add; // ip + porta udp
    unsigned int pass;  //da controllare che sia tra 0 e 65535

    unsigned int nMsg;  //INUTILE CREARE MACRO CHE SIA FINALE (ES. #define END_MESSAGE nullptr)---------------------------------------------
    msg * listMsg;

    //STESSA COSA PER QUESTI QUA SOTTO MA VEDERE SE CREARE STRUCT SEPARATA PER TABELLA DATO CHE IN CLIENT NON CI DEVE STARE-------------------------------------------
    unsigned int nFri;
    char  listFri [MAX_CLIENT-1] [ID_LEN]; // dato che il server ha un massimo di 100 user, 
                        // può avere un massimo 99 amici 
};


//-------------------------------TUTTO QUELLO SOPRA METTERE IN LIB COMUNE----------------------------------------------------
//-------------------------------TUTTO QUELLO SOTTO SPECIFICO CLIENT---------------------------------------------------------

//Gestisce l'avvio del client, nello specifico all'avvio viene controllato il flusso di messaggi in ingresso per controllare novità.
//Se il client non è registrato dovrà registrarsi, se già registrato procederà ad accedere (prevedere salvataggio credenziali facoltativo senza criptazione per semplicità)
//o chiederndo password o solo username (piccoli cookies?).
//sono presenti due socket uno per la comunicazione con il serve tcp, e uno per la gestione dei flussi udp
void startup();

//Gestisce spegnimento del client
void  shutdown();