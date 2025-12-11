/*
    Client.c

    main del client

    accetta opzione -d per debug
                    -c per credenziali usare di default

    per compilare gcc -Wall Client.c libClient.c -o client
*/

#include "Client.h"

void stampaMenu(){
    printf("\ninserisci un numero corrispondente alla funzionalità\n1 registrarsi\n2 accedere\n3 inviare una richiesta d'amicizia\n4 inviare un messaggio\n5 invia un messaggio alla tua rete\n6 visiona gli utenti attivi\n7 visiona le tue notifiche\n8 esci\n");
}

int main(const int argc, const char* args[])
{
    int err = NOTOK;    //variabile per controllo valore ritorno funzioni

    debug("Client: Inizio inizializzazione\n");    //debug

    err = initialization(argc, args);

    if( err != OK)
    {
        debug("Client: inizializzazione fallita\n");    //debug

        printf("inizializzazione fallita\n");

         return NOTOK;

         //TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    }
    
    debug("Client: inizializzazione risucita\n");    //debug


    debug("Client: Inizio startup\n"); //debug
    
    err = startup();   //avvio client

    if(err != OK){  //controllo errore
        //errore
        printf("errore avvio client\n");

        return NOTOK;

        //TODO.-----------------------------------------------------------------------------------------------------------------
    }

    debug("Client: fine startup\n");    // debug
    
    printf("Avvio del client riuscito\n");
    uint8_t sceltaUtente;

    while(true)
    {
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

        case 2 :

            err = login();
            if(err != OK)
                printf("accesso fallito\n");
            else
                printf("accesso riuscito\n");
            break;

        case 3:

            //cattura input
            char idClientFriendRequest[DIMBUF];
            printf("inserisci l'username del client di cui desideri diventare amico\n");
            scanf("%s", idClientFriendRequest);

            err = friend_request(idClientFriendRequest);
            if(err != OK)
                printf("invio richiesta d'amicizia fallito\n");
            else
                printf("invio richiesta d'amicizia riuscito\n");
            break;

        case 8 :
            return OK;

        default:

            printf("scelta inserita non valida, inserisci un numero da 1 a 8\n");
            
            break;
        }
    }
}