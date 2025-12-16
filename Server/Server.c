#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>



typedef struct sockaddr * pSockaddr;
unsigned int DEBUG = 0;

int main (int argc, char * argv[]){
    //aggiungi controllo per avviare il main server in modalita debug 
  
    if(argc >= 2){
        if(strcmp(argv[1], "-d") == 0){
            
            DEBUG=1;
        }
        else{
            printf("Parametro non riconosciuto, Ricordo \"-d\" per avviare modalità Debug\n");
            return EXIT_FAILURE;
        }
    }
    
    struct sockaddr_in sockServer;
    int sock_server;
    int r;

    if(DEBUG) printf("Avvio serverInit()\n");

    //inalizazione di tutti gli insieme dei semafori e del socket UDP
    if (serverInit(DEBUG) == NOTOK) { 
        fprintf(stderr, "Errore durante l'inizializzazione del server.\n");
        return EXIT_FAILURE;
    }

    if(DEBUG) printf("Socket()\n");
    
    sock_server =socket(PF_INET,SOCK_STREAM,0);
    if(sock_server == -1){
        perror("Errore in socket()");
        serverClose();
        return EXIT_FAILURE;
    }

    sockServer.sin_family=AF_INET;
    sockServer.sin_port=htons(SERVER_PORT);
    sockServer.sin_addr.s_addr=htonl(INADDR_ANY);


    if(DEBUG) printf("Bind()\n");


    r = bind(sock_server,(pSockaddr)&sockServer,sizeof(struct sockaddr_in));
    if(r < 0){
        perror("Errore in bind()");
        serverClose();
        close(sock_server);
        return EXIT_FAILURE;
    }

    if(DEBUG) printf("listen()\n");
    r = listen(sock_server, 0);
    if(r == 0 ){
        struct sockaddr_in sockClient;
        socklen_t si = sizeof(sockClient);
       

        while (1){
            int *  sock_client = (int *) malloc(sizeof(int));

            if(DEBUG) printf("Attesa di client: ....\n");

            *sock_client = accept(sock_server, (pSockaddr) &sockClient, &si);

            if(DEBUG) printf("client accept, passaggio al thread\n");


            if(sock_client >= 0){
                pthread_t threadClient;
                pthread_create(&threadClient, NULL, pthreadConection, sock_client);
            }
            
        }
        
    }


    serverClose();
    return EXIT_SUCCESS;
}