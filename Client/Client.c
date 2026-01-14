/*
    Client.c

    main del client

    accetta opzione -d per debug
                    -c per credenziali usare di default
                    -p per impostare il valore della porta UDP, non verrà salvata su file
                    -? help
*/

#include "Client.h"

void stampaMenu(bool sePrimaVolta)
{
    if (sePrimaVolta)
        printf("\ninserisci un numero corrispondente alla funzionalità\n1 registrarsi\n2 accedere\n8 esci\n");
    else
        printf("\n3 inviare una richiesta d'amicizia\n4 inviare un messaggio\n5 invia un messaggio alla tua rete\n6 visiona gli utenti attivi\n7 visiona le tue notifiche\n8 esci\n");
}

int main(const int argc, const char *args[])
{
    int err = NOTOK; // variabile per controllo valore ritorno funzioni

    debug("Client: Inizio inizializzazione\n"); // debug

    err = initialization(argc, args);

    if (err != OK)
    {
        debug("Client: inizializzazione fallita\n"); // debug

        printf("inizializzazione fallita\n");

        return NOTOK;

        // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    }

    debug("Client: inizializzazione riuscita\n"); // debug

    debug("Client: Inizio startup\n"); // debug

    err = startup(); // avvio client

    if (err != OK)
    { // controllo errore
        // errore
        printf("errore avvio client\n");

        return NOTOK;

        // TODO.-----------------------------------------------------------------------------------------------------------------
    }

    debug("Client: fine startup\n"); // debug

    printf("Avvio del client riuscito\n");
    uint8_t sceltaUtente;
    bool primaVolta = true;
    bool errprimoAccesso = false;

    while (true)
    {
        
        stampaMenu((primaVolta || errprimoAccesso));
        scanf("%hhd", &sceltaUtente);

        switch (sceltaUtente)
        {
        case 1:
            {
                err = new_client(); //chiamata newClient
                if (err != OK)
                {
                    printf("registrazione fallita\n");
                    errprimoAccesso = true;
                }
                else{
                    printf("registrazione riuscita\n");
                    errprimoAccesso = false;
                }
                break;
            }
        case 2:
            {
                err = login();  //chiamata login
                if (err != OK)
                {
                    printf("accesso fallito\n");
                    errprimoAccesso = true;
                }
                else{
                    errprimoAccesso = false;
                    printf("accesso riuscito\n");
                }
                break;
            }
        case 3:
            {
                // cattura input
            char idClientFriendRequest[ID_LEN]; // id client a cio inviare la richiesta d'amicizia

            // input/outout
            printf("inserisci l'username del client di cui desideri diventare amico\n");
            scanf("%s", idClientFriendRequest);

            // verifica errore
            err = friend_request(idClientFriendRequest);    //chiamata friendRequest
            if (err != OK)
                printf("invio richiesta d'amicizia fallito\n");
            else
                printf("invio richiesta d'amicizia riuscito\n");
            break;
            }
        case 4:
            {
                // cattura input
                char idClintDestination[ID_LEN]; // id client a cui inviare la richiesta
                char message[201];               // buffer del messaggio
    
                // input/outout
                printf("inserisci l'username del client a cui desideri inivare il messaggio\n");
                scanf("%s", idClintDestination);
    
                // input/outout
                printf("inserisci il messaggio da inviare:\n");
                scanf("%s", message);
    
                err = send_message(idClintDestination, message);    //chiamata sendMessage
                if (err != OK)
                    printf("invio messaggio fallito\n");
                else
                    printf("invio messaggio riuscito\n");
                break;
            }
        case 5:
            {
                char message[201];               // buffer del messaggio

                 // input/outout
                printf("inserisci il messaggio da inviare:\n");
                scanf("%s", message);
    

                err = flood( message);  //chiamata flood
                if (err != OK)
                    printf("invio richiesta flood fallito\n");
                else
                    printf("invio richiesta flood riuscito\n");
                break;
            }
        case 6 :
        {
            char listClient[DIMBUF];               // buffer della lista dei clint

            err = list_client(listClient);  //chiamata listClient
            if (err != OK)
                printf("invio richiesta numero client fallito\n");
            else{
                printf("invio richiesta numero client riuscito\n");

                // outout
                printf("id dei client:%s\n", listClient);
            }
            break;
        }
        case 7 :
        {
            char text[DIMBUF];               // buffer della lista dei clint

            err = read_notify(text);  //chiamata listClient
            bool flag = false;   //variabile flag del ciclo, messa a falso quando bisognerà uscire e quindi l'utente avrà risposto, tiene conto se è una richiesta d'amicizia EIRF

            if(err == FRIEND_REQUEST){  //ricevuta richiesta d'amicizia
                // outout
                printf("%s\n", text);
                
                flag = true;
                char userResponse;

                while (flag)
                {
                    // input/output
                    printf("vuoi accettare? (y/n)\n");
                    scanf("%c", &userResponse);

                    switch (userResponse)
                    {
                    case 'y':
                    {
                        //accetta la richiesta
                        err = friend_request_response(userResponse);
                        flag = false;
                        break;
                    }
                    case 'n':
                    {
                        //rifiuta la richiesta
                        err = friend_request_response(userResponse);
                        flag = false;
                        break;
                    }
                    default:
                    {
                        // output
                        printf("risposta non riconosciuta, riprova\n");
                        break;
                    }
                    }
                }

                flag = true;
            }

            //controllo errori friend_request_response/read_notify
            if (err != OK)
                printf("invio richiesta consultazione notifiche fallito\n");
            else if (flag){
                printf("Invio risposta richiesta d'amicizia riuscito\n");
            } else 
                printf("%s", text);

            
            
            
            break;;
        }
        case 8:
        {
            err = client_shutdown();
            if (err != OK)
            {
                printf("disconnessione fallita\n");
                return NOTOK;
            }
            else
                printf("disconnessione riuscita\n");
            return OK;
        } 
        default:
            {
                printf("scelta inserita non valida, inserisci un numero da 1 a 8\n");

                break;
            } 
        }

        primaVolta = false;
    }
}