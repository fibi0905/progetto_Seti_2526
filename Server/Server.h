#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_CLIENT 100
#define ID_LEN 9
#define MAX_LEN 201


typedef struct user user;
typedef struct msg msg;

typedef enum typFlux  typFlux;

enum typFlux{
    FRIE_Req,
    FRIE_A,
    FRIE_R,
    MSG,
    FLOO,
};

struct user{
    char id [ID_LEN];
    struct sockaddr_in add; // ip + porta udp
    unsigned int pass;

    unsigned int nMsg;
    msg * listMsg;

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