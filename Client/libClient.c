/*
    libClient.c

    Definizione di tutte le funzioni presenti in Client.h.
    Aggiungi di funzioni usate internamente dal client.
*/
#include "Client.h"

#define PASSWORD_STD 0
#define ID_STD "Ferraro"

// variabili globali
user utente; // descrittore utente

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
        strcpy(utente.pw, PASSWORD_STD);

        // debug
        if (DEBUG)
        {
            printf("Client: sarà effettuto un login con credenziali fittizione std\n");
        }

        return OK;
    }
    // TODO------------------------------------------------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int startup()
{
    // debug
    if (DEBUG)
    {
        printf("Client: Inizio creazione socket TCP\n");
    }

    int vetDesc[2] = {-1, -1}; // usato per memorizzare i descrittori dei client, inizializzato a -1

    // creazione socket tcp per comunicazione con server
    struct sockaddr_in socketTCP;                     // allocazione socket
    socketTCP.sin_family = AF_INET;                   // socket ipv4
    socketTCP.sin_port = htons(PORTA_SERVER);         // porta aperta
    inet_aton(INDIRIZZO_SERVER, &socketTCP.sin_addr); // indirizzo server

    int descrTCP = socket(PF_INET, SOCK_STREAM, 0); // creazione socket TCP ipv4

    // debug
    if (DEBUG)
    {
        printf("Client: tentativo connessione server\n");
    }

    int r = connect(descrTCP, (struct sockaddr *)&socketTCP, sizeof(struct sockaddr_in)); // creazione connessione

    if (r == -1)
    { // controllo se connessione avvenuta con successo
        // Connessione fallita

        // debug
        if (DEBUG)
        {
            printf("Client: connessione fallita\n");
        }

        vetDesc[0] = descrTCP;
        vetDesc[1] = NULL;
        shutdown(vetDesc);

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
        printf("Client: creazioen socket UDP\n");
    }

    // creazione socket udp per comunicazione con server (gestione notifiche)
    struct sockaddr_in socketUDP;                     // allocazione socket
    socketUDP.sin_family = AF_INET;                   // socket ipv4
    socketUDP.sin_port = htons(PORTA_SERVER);         // porta aperta
    inet_aton(INDIRIZZO_SERVER, &socketUDP.sin_addr); // indirizzo server

    int descrUDP = socket(PF_INET, SOCK_DGRAM, 0); // creazione socket udp ipv4
    // niente connect dato che connessione non affidabile

    // debug
    if (DEBUG)
    {
        printf("Client: creazione socket udp riuscita\n");
    }

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

    if (recuperoCredenziali(true) != OK){
        //errore
        //TODO----------------------------------------------------------------------------------------------
    }

    msg messaggio;
    strcpy(messaggio.id, utente.id);
    strcpy()
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void shutdown(int *desc)
{
    // TODO---------------------------------------------------------------------------------------------------------
    if (desc[0] != -1)
        close(desc[0]); // chiusura socket udp ipv4
    if (desc[1] != -1)
        close(desc[1]); // chiusura socket tcp ipv4
}
