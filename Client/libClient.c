/*
    libClient.c

    Definizione di tutte le funzioni presenti in Client.h.
    Aggiungi di funzioni usate internamente dal client.
*/
#include "Client.h"

#define PASSWORD_STD 0
#define ID_STD "Ferraro"

// variabili globali
user utente;  // descrittore utente
int descrTCP; // descrittore socekt TCP
int descrUDP; // descrittore socket UDP

// gestisce recupero creadenziali, può recuperarle da file, o da riga terminale.
// fare in modo che possa salvare le credenziali l'utente
// il parametro test se vero procedere con le credenziali fittizie
//
// per il momento solo con test true
int recuperoCredenziali(bool test)
{
    // debug
    if (DEBUG)
    {
        printf("Client: inizio login\n");
    }

    if (test)
    {
        strcpy(utente.id, ID_STD);
        utente.mdp = PASSWORD_STD;

        // debug
        if (DEBUG)
        {
            printf("Client: sarà effettuto un login con credenziali fittizione std\n");
        }

        return OK;
    }

    return NOTOK;
    // TODO------------------------------------------------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int startup()
{
    // inizializzazione descrittori socket
    descrTCP = NOTOK;
    descrUDP = NOTOK;

    // debug
    if (DEBUG)
    {
        printf("Client: Inizio creazione socket TCP\n");
    }

    // creazione socket tcp per comunicazione con server
    struct sockaddr_in socketTCP;                     // allocazione socket
    socketTCP.sin_family = AF_INET;                   // socket ipv4
    socketTCP.sin_port = htons(PORTA_SERVER);         // porta aperta
    inet_aton(INDIRIZZO_SERVER, &socketTCP.sin_addr); // indirizzo server

    descrTCP = socket(PF_INET, SOCK_STREAM, 0); // creazione socket TCP ipv4

    // debug
    if (DEBUG)
    {
        printf("Client: tentativo connessione server\n");
    }

    int r = connect(descrTCP, (struct sockaddr *)&socketTCP, sizeof(struct sockaddr_in)); // creazione connessione

    if (r == -1) // controllo se connessione avvenuta con successo
    {
        // Connessione fallita

        // debug
        if (DEBUG)
        {
            printf("Client: connessione fallita\n");
        }

        client_shutdown();

        return NOTOK;
        // TODO--------------------------------------------------------------------------------------------------------------------------
    }
    // connessione riuscita

    // debug
    if (DEBUG)
    {
        printf("Client: connessione riuscita\n");
    }

    // debug
    if (DEBUG)
    {
        printf("Client: creazione socket UDP\n");
    }

    // creazione socket udp per comunicazione con server (gestione notifiche)
    struct sockaddr_in socketUDP;                 // allocazione socket
    socketUDP.sin_family = AF_INET;               // socket ipv4
    socketUDP.sin_port = htons(PORTA_UDP_CLIENT); // porta aperta
    inet_aton(INADDR_ANY, &socketUDP.sin_addr);   // indirizzo in ascolto (tutti)

    descrUDP = socket(PF_INET, SOCK_DGRAM, 0); // creazione socket udp ipv4

    // niente connect dato che connessione non affidabile

    // debug
    if (DEBUG)
    {
        printf("Client: creazione socket udp riuscito\n");
    }

    // debug
    if (DEBUG)
    {
        printf("Client: bind del socket udp\n");
    }

    r = bind(descrUDP, (struct sockaddr*) &socketUDP, sizeof(struct sockaddr_in)); // bind dato che dobbiamo ricevere notifiche

    // debug
    if (DEBUG)
    {
        printf("Client: bind del socket udp riuscio\n");
    }

    if (r == -1) // controllo bind avvenuta con successo
    {
        // bind fallita

        // debug
        if (DEBUG)
        {
            printf("Client: bind del socket udp fallito\n");
        }

        return NOTOK;
        // TODO----------------------------------------------------------------------------------
    }

    // bind riuscita

    utente.port = PORTA_UDP_CLIENT; // riempo struttura user con porta udp

    return OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int newClient()
{
    // debug
    if (DEBUG)
    {
        printf("Client: Inizio registrazione\n");
    }

    // debug
    if (DEBUG)
    {
        printf("Client: recupero credenziali\n");
    }

    if (recuperoCredenziali(true) != OK)
    {
        // errore

        // debug
        if (DEBUG)
        {
            printf("Client: recupero credenziali fallito\n");
        }

        return NOTOK;
        // TODO----------------------------------------------------------------------------------------------
    }

    // debug
    if (DEBUG)
    {
        printf("Client: recupero credenziali riuscito\n");
    }

    // debug
    if (DEBUG)
    {
        printf("Client: invio messaggio REGIS\n");
    }

    // creo buffer e riempo con messaggio finale
    char msg[BUFSIZ];
    snprintf(msg, sizeof(msg), "REGIS %s %d %d+++", utente.id, utente.port, utente.mdp);

    // debug
    if (DEBUG)
    {
        printf("Client: messaggio:\"%s\"\n", msg);
    }

    if (write(descrTCP, msg, strlen(msg)) <= 0) // controllo numero byte scritti
    {
        // byte scritto minori di 1, errore

        // debug
        if (DEBUG)
        {
            printf("Client: invio messsaggio REGIS fallito\n");
        }

        return NOTOK;
        // TODO--------------------------------------
    }

    // debug
    if (DEBUG)
    {
        printf("Client: invio messsaggio REGIS riuscito\n");
    }

    // byte scritti sufficienti

    // debug
    if (DEBUG)
    {
        printf("Client: lettura risposta server\n");
    }

    if (read(descrTCP, msg, sizeof(char) * BUFSIZ) <= 0) // lettura risposta server e controllo numero di byte letti
    {
        // byte letti insufficienti

        // debug
        if (DEBUG)
        {
            printf("Client: lettura risposta server fallita\n");
            printf("Client: messaggio server:\"%s\"", msg);
        }

        return NOTOK;

        // TODO--------------------------------------------------------------------------------------------------
    }

    // debug
    if (DEBUG)
    {
        printf("Client: lettura risposta server riuscito\n");
        printf("Client: messaggio server:\"%s\"", msg);
    }

    if (strcmp(msg, "WELCO+++")) // se accetta registrazione
    {
        // debug
        if (DEBUG)
        {
            printf("Client: registrazione avvenuta con successo\n");
        }

        return OK;
    }

    // registrazione rifiutata

    // debug
    if (DEBUG)
    {
        printf("Client: registrazione fallita\n");
    }

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void client_shutdown()
{
    close(descrTCP);
    close(descrUDP);
    // TODOOOOOOOO-----------------------------------------------------------
}
