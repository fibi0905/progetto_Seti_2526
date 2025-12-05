#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>  
#include <arpa/inet.h>   
#include <sys/socket.h>  
#include <string.h>

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
    char id [9];
    struct sockaddr_in add; // ip + porta udp
    unsigned int pass;

    unsigned int nMsg;
    msg * listMsg;

    unsigned int nFri;
    char  listFri [99] [9]; // dato che il server ha un massimo di 100 user, 
                        // può avere un massimo 99 amici 
};

struct msg{
    char id [9];
    char  msg [201]; //200 caratteri + \0
    typFlux typeMSG; //gestire il tipo.
    msg * next;
}