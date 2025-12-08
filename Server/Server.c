#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#define SERVER_PORT 8080

typedef struct sockaddr * pSockaddr;

int main (int argc, char * argv[]){
    //aggiungi controllo per avviare il main server in modalita debug ù
    
    struct sockaddr_in sockServer;
    int sock_server;
    int r;

    //inalizazione di tutti gli insieme dei semafori e del socket UDP
    if (serverInit() == NOTOK) { 
        fprintf(stderr, "Errore durante l'inizializzazione del server.\n");
        return EXIT_FAILURE;
    }

    
    sock_server =socket(PF_INET,SOCK_STREAM,0);
    if(sock_server == -1){
        perror("Errore in socket()");
        serverClose();
        return EXIT_FAILURE;
    }


    sockServer.sin_family=AF_INET;
    sockServer.sin_port=htons(SERVER_PORT);
    sockServer.sin_addr.s_addr=htonl(INADDR_ANY);


    r = bind(sock_server,(pSockaddr)&sockServer,sizeof(struct sockaddr_in));
    if(r < 0){
        perror("Errore in bind()");
        serverClose();
        close(sock_server);
        return EXIT_FAILURE;
    }

    r = listen(sock_server, 0);
    if(r == 0 ){
        struct sockaddr_in sockClient;
        socklen_t si = sizeof(sockClient);

        while (1){
            int *  sock_client = (int *) malloc(sizeof(int));
            *sock_client = accept(sock_server, (pSockaddr) &sockClient, &si);
            if(sock_client >= 0){
                pthread_t threadClient;
                pthread_create(&threadClient, NULL, pthreadConection, sock_client);
            }
        }
        
    }


    serverClose();
    return EXIT_SUCCESS;
}