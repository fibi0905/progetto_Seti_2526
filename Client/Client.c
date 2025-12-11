/*
    Client.c

    main del client

    per compilare gcc -Wall Client.c libClient.c -o client
*/

#include "Client.h"


void stampaMenu(){
    printf("inserisci un numero corrispondente alla funzionalità\n1 registrarsi\n2 accedere\n3 inviare una richiesta d'amicizia\n4 inviare un messaggio\n5 invia un messaggio alla tua rete\n6 visiona gli utenti attivi\n7 visiona le tue notifiche\n8 esci");
}

int main(){
    //debug
    if(DEBUG){
        printf("Client: Inizio startup\n");
    }

    int err = startup();

    if(err != OK){  //controllo errore
        //errore
        //TODO.-----------------------------------------------------------------------------------------------------------------
    }

    //debug
    if(DEBUG){
        printf("Client: fine startup\n");
    }
    
    printf("Avvio del client riuscito\n");
    uint8_t sceltaUtente;

    while(true){
        stampaMenu();
        scanf("%hhd", &sceltaUtente);
        switch (sceltaUtente)
        {
        case 1 :
            err = newClient();
            if(err != OK)
                printf("registrazione fallita\n");
            else
                printf("registrazione riuscita\n");
            break;
        
        default:
            break;
        }
    }
}