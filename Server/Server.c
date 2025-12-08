#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#define SERVER_PORT 8080

int main (int argc, char * argv[]){
    //aggiungi controllo per avviare il main server in modalita debug ù
    
    struct sockaddr_in sockaddress;
    int sock;
    int r;

    //inalizazione di tutti gli insieme dei semafori e del socket UDP
    if (server_init() != 0) { 
        fprintf(stderr, "Errore durante l'inizializzazione del server.\n");
        return EXIT_FAILURE;
    }

    
    sock=socket(PF_INET,SOCK_STREAM,0);


    sockaddress.sin_family=AF_INET;
    sockaddress.sin_port=htons(SERVER_PORT);
    sockaddress.sin_addr.s_addr=htonl(INADDR_ANY);


    r=bind(sock,(struct sockaddr *)&sockaddress,sizeof(struct sockaddr_in));



}