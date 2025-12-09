#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include "../src/utility.h"

#define MAX_CLIENT 100
#define ID_LEN 9
#define MAX_LEN 201

#define OK 1
#define NOTOK 0
#define NOTFIND -1


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

struct user{
    char id [ID_LEN];
    struct sockaddr_in add; // ip + porta udp
    unsigned int pass;

    unsigned int nMsg;
    msg * listMsg; // ?Possiblie controllo sul numero massimo?

    unsigned int nFri;
    char  listFri [MAX_CLIENT-1] [ID_LEN]; // dato che il server ha un massimo di 100 user, 
                        // può avere un massimo 99 amici 
};

struct msg{
    char id [ID_LEN];
    char  value [MAX_LEN]; //200 caratteri + \0
    typFlux typeMSG; //gestire il tipo.
    msg * next;
};




//funzioni gestione sever 
unsigned int serverInit ();
void * pthreadConection (void * sockClient);
void serverClose();

