/*
    libClient.c

    Definizione di tutte le funzioni presenti in Client.h.
    Aggiungi di funzioni usate internamente dal client.
*/
#include "Client.h"

//----------------VARIABILI GLOBALI-------------------------------------------------------------------------------------------------------------------------------------
user utente;               // descrittore utente
int descrTCP;              // descrittore socekt TCP
int descrUDP;              // descrittore socket UDP
bool seDebugAttivo;        // attiva modalità verbose
bool seCredenzialiDefault; // attiva le credenziali di default per i test

//-------------------INIZIO FUNZIONI------------------------------------------------------------------------------------------------------------------------------------

int initialization(const int argc, const char *args[])
{
    // inizializzazione variabili globali
    descrTCP = NOTOK;
    descrUDP = NOTOK;
    seDebugAttivo = false;
    seCredenzialiDefault = false;

    for (unsigned int i = 1; i < argc; i++)
    {
        if (strcmp(args[i], "-d") == 0)
            set_debug(true); // attivazione modalità verbose
        else if (strcmp(args[i], "-c") == 0)
            set_credenzialiDefault(true); // attivazione uso credenziali predefinite
        else
        {
            // Inserita opzione non riconosciuta

            debug("Client: opzioni non valide, inserito:\"%s\"\n", args[i]);

            return NOTOK;
        }
    }

    return OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_debug(const bool b)
{
    seDebugAttivo = b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_credenzialiDefault(bool b)
{
    seCredenzialiDefault = b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gestisce recupero creadenziali, può recuperarle da file, o da riga terminale.
// fare in modo che possa salvare le credenziali l'utente
// il parametro test se vero procedere con le credenziali fittizie
//
// per il momento solo con test true
// NON TERMINATA
int recuperoCredenziali()
{
    debug("Client: inizio recupero credenziali\n"); // debug

    if (seCredenzialiDefault)
    {
        strcpy(utente.id, ID_STD);
        utente.mdp = PASSWORD_STD;

        debug("Client: sarà effettuto un login con credenziali fittizione std\n"); // debug

        return OK;
    }

    return NOTOK;
    // TODO------------------------------------------------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool seBufferVuoto(int fd)
{
    debug("Client: inizio controllo se buffer vuoto\n"); // debug
    char buf;
    ssize_t ret;

    // Tenta di leggere 1 byte senza rimuoverlo dal buffer
    ret = recv(fd, &buf, 1, MSG_PEEK | MSG_DONTWAIT);

    debug("Client: il buffer ha estratto \"%c\", numero byte letti %d, valore di ritorno: %d\n", buf, ret, ret == -1); // debug

    return (ret == -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void svuota_buffer(const int socket_fd)
{
    char trash[256]; // Variabile spazzatura
    int n;

    // MSG_DONTWAIT: Legge se c'è roba. Se è vuoto, NON si blocca e ritorna -1.
    while ((n = recv(socket_fd, trash, sizeof(trash), MSG_DONTWAIT)) > 0)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void debug(const char *format, ...)
{
    if (seDebugAttivo)
    {
        va_list args;

        // Inizializza la lista degli argomenti variabili
        va_start(args, format);

        // Stampa usando vprintf (v = variadic)
        vprintf(format, args);

        // Pulisce la memoria della lista
        va_end(args);

        fflush(stdout); // per essere sicuri di aver scritto tutto
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NON TERMINATA
int startup()
{
    debug("Client: Inizio creazione socket TCP\n"); // debug

    // creazione socket tcp per comunicazione con server
    struct sockaddr_in socketTCP;                     // allocazione socket
    socketTCP.sin_family = AF_INET;                   // socket ipv4
    socketTCP.sin_port = htons(SERVER_PORT);          // porta aperta
    inet_aton(INDIRIZZO_SERVER, &socketTCP.sin_addr); // indirizzo server

    descrTCP = socket(PF_INET, SOCK_STREAM, 0); // creazione socket TCP ipv4

    debug("Client: tentativo connessione server\n"); // debug

    int r = connect(descrTCP, (struct sockaddr *)&socketTCP, sizeof(struct sockaddr_in)); // creazione connessione

    if (r == -1) // controllo se connessione avvenuta con successo
    {
        // Connessione fallita

        debug("Client: connessione fallita\n"); // debug

        client_shutdown();

        return NOTOK;
        // TODO--------------------------------------------------------------------------------------------------------------------------
    }
    // connessione riuscita

    debug("Client: connessione riuscita\n"); // debug

    debug("Client: creazione socket UDP\n"); // debug

    // creazione socket udp per comunicazione con server (gestione notifiche)
    struct sockaddr_in socketUDP;                  // allocazione socket
    socketUDP.sin_family = AF_INET;                // socket ipv4
    socketUDP.sin_port = htons(PORTA_UDP_CLIENT);  // porta aperta
    socketUDP.sin_addr.s_addr = htons(INADDR_ANY); // indirizzo in ascolto (tutti)

    descrUDP = socket(PF_INET, SOCK_DGRAM, 0); // creazione socket udp ipv4

    // niente connect dato che connessione non affidabile

    debug("Client: creazione socket udp riuscito\n"); // debug

    debug("Client: bind del socket udp\n"); // debug

    r = bind(descrUDP, (struct sockaddr *)&socketUDP, sizeof(struct sockaddr_in)); // bind dato che dobbiamo ricevere notifiche

    if (r == -1) // controllo bind avvenuta con successo
    {
        // bind fallita

        debug("Client: bind del socket udp fallito\n"); // debug

        return NOTOK;
        // TODO----------------------------------------------------------------------------------
    }

    // bind riuscita
    debug("Client: bind del socket udp riuscio\n"); // debug

    utente.port = PORTA_UDP_CLIENT; // riempo struttura user con porta udp

    return OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NON TERMINATA
int newClient()
{
    debug("Client: Inizio registrazione\n"); // debug

    debug("Client: recupero credenziali\n"); // debug

    if (recuperoCredenziali() != OK)
    {
        // errore

        debug("Client: recupero credenziali fallito\n"); // debug

        return NOTOK;
        // TODO----------------------------------------------------------------------------------------------
    }

    debug("Client: recupero credenziali riuscito\n"); // debug

    debug("Client: invio messaggio REGIS\n"); // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "REGIS ", 6); 
    offset += 6;    // Spostiamo offset avanti di 6
    

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 9, "%-8s", utente.id); //uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset+= 8;
    msg[offset] = ' '; 
    offset += 1;

    // %04d : usa 5 byte, l'ultimo per il '\0'
    snprintf(msg + offset, 5, "%04d", utente.port);
    offset += 4;
    msg[offset] = ' '; 
    offset += 1;

    //aggiungo al messaggio la password in little endian
    msg[offset] = (char)(utente.mdp & 0xFF);            // Byte basso
    msg[offset + 1] = (char)((utente.mdp >> 8) & 0xFF); // Byte alto
    offset += 2;

    //aggiungo "+++"
    memcpy(msg + offset, "+++", 3); 
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte <= 0) // controllo numero byte scritti
    {
        // byte scritto minori di 1, errore

        debug("Client: invio messsaggio REGIS fallito, scritti %d\n", nByte); // debug

        return NOTOK;
        // TODO--------------------------------------
    }

    debug("Client: invio messsaggio REGIS riuscito, inviati %d\n", nByte); // debug

    // byte scritti sufficienti

    debug("Client: lettura risposta server\n"); // debug

    // lettura risposta server e controllo numero di byte letti
    nByte = read(descrTCP, msg, sizeof(char) * DIMBUF);
    if (nByte != 8)
    {
        // byte letti insufficienti

        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server fallita, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                     // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;

        // TODO--------------------------------------------------------------------------------------------------
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "WELCO+++") == 0) // se accetta registrazione
    {
        if (seBufferVuoto(descrTCP))
        {
            debug("Client: registrazione avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "GOBYE+++") == 0)
    {
        // registrazione rifiutata
        if (seBufferVuoto(descrTCP))
            debug("Client: registrazione rifiutata con successo\n"); // debug

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;
    }

    // registrazione fallita

    debug("Client: registrazione fallita\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NON TERMINATA
int login()
{
    debug("inizio login");

    if (recuperoCredenziali() != OK) // per avere dentro struttura user i dati corretti
    {
        debug("Client: recupero credenziali fallito");

        return NOTOK;

        // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    }

    debug("Client: invio messaggio CONNE\n"); // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;
    //snprintf(msg, sizeof(msg), "CONNE %s %d+++", utente.id, utente.mdp);

    memcpy(msg + offset, "CONNE ", 6); 
    offset += 6;    // Spostiamo offset avanti di 6

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 9, "%-8s", utente.id); //uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset+= 8;
    msg[offset] = ' '; 
    offset += 1;

    // %04d : usa 5 byte, l'ultimo per il '\0'
    snprintf(msg + offset, 5, "%04d", utente.port);
    offset += 4;
    msg[offset] = ' '; 
    offset += 1;

    //aggiungo al messaggio la password in little endian
    msg[offset] = (char)(utente.mdp & 0xFF);            // Byte basso
    msg[offset + 1] = (char)((utente.mdp >> 8) & 0xFF); // Byte alto
    offset += 2;

    //aggiungo "+++"
    memcpy(msg + offset, "+++", 3); 
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    debug("Client: invio messsaggio CONNE riuscito, inviati %d\n", nByte); // debug

    nByte = write(descrTCP, msg, offset);
    if (nByte <= 0) // controllo numero byte scritti
    {
        // byte scritto minori di 1, errore

        debug("Client: invio messsaggio CONNE fallito, scritti %d\n", nByte); // debug

        return NOTOK;
        // TODO--------------------------------------
    }

    debug("Client: invio messsaggio CONNE riuscito, inviati %d\n", nByte); // debug

    // byte scritti sufficienti

    debug("Client: lettura risposta server\n"); // debug

    // lettura risposta server e controllo numero di byte letti
    nByte = read(descrTCP, msg, sizeof(char) * DIMBUF);
    if (nByte != 8)
    {
        // byte letti errati

        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server fallita, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                     // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;

        // TODO--------------------------------------------------------------------------------------------------
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "HELLO+++") == 0) // se accetta login
    {

        if (seBufferVuoto(descrTCP))
        {
            debug("Client: login avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "GOBYE+++") == 0)
    {
        // login rifiutata

        if (seBufferVuoto(descrTCP))
            debug("Client: login rifiutata con successo\n"); // debug

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        return NOTOK;
    }

    // login fallita

    debug("Client: login fallita\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int friend_request(char* id)
{
    debug("Clinet: inizio richiesta d'amicizia\n"); //debug

    

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NON TERMINATA
void client_shutdown()
{
    close(descrTCP);
    close(descrUDP);
    // TODOOOOOOOO-----------------------------------------------------------
}
