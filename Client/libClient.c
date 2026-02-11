/*
    libClient.c

    Definizione di tutte le funzioni presenti in Client.h.
    Aggiungi di funzioni usate internamente dal client.
*/
#include "Client.h"

//----------------VARIABILI GLOBALI-------------------------------------------------------------------------------------------------------------------------------------
user utente;               // descrittore utente
int descrTCP;              // descrittore socket TCP
int descrUDP;              // descrittore socket UDP
pthread_t tid;             // id del thread
bool seDebugAttivo;        // attiva modalità verbose
bool seCredenzialiDefault; // attiva le credenziali di default per i test
bool seErrore;             // se vero c'è stato un errore critico

//----------------INIZIO FUNZIONI "PRIVATE"-----------------------------------------------------------------------------------------------------------------------------

// Verifica che il buffer sia vuoto
bool se_bufferVuoto(int fd)
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

// effettua la pulizia del buffer del fd passato per argomento
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

// gestisce recupero creadenziali da terminale.
// il parametro test se vero procedere con le credenziali fittizie
int recupero_credenziali()
{
    debug("Client: inizio recupero credenziali\n"); // debug

    if (seCredenzialiDefault)
    {
        strcpy(utente.id, ID_STD);
        utente.mdp = PASSWORD_STD;

        debug("Client: sarà effettuto un login con credenziali fittizione std\n"); // debug

        return OK;
    }

    // inizio registrazione/login utente
    printf("----REGISTRAZIONE/LOGIN----\n");
    do
    {
        printf("inserisci l'username (8 caratteri alfanumerici):\n");
        scanf("%s", utente.id);

        // controllo lunghezza username
        if (strlen(utente.id) != 8)
            printf("ERRORE: l'username deve essere formato da 8 caratteri alfanumerici\n");
    } while (strlen(utente.id) != 8);

    do
    {
        printf("inserisci la password(compresa tra 0 e 65535):\n");
        scanf("%d", &utente.mdp);

        // controllo grandezza passoword
        if (utente.mdp < 0 || utente.mdp > 65535)
            printf("ERRORE: la password deve essere compresa tra 0 e 65535\n");
    } while (strlen(utente.id) != 8);

    return OK;
}

//-------------------INIZIO FUNZIONI THREAD-----------------------------------------------------------------------------------------------------------------------------

void* notificheUDP(void* args)
{
    debug("Thread: avviato con successo, tid=%d\n", tid);    //debug

    // Abilito la cancellazione
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    
    // Imposto il tipo "asincrona": il thread termina quando viene chiamata una pthread_cancel
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    // creo buffer
    char buf[3];

    int nByte = 0; // contatore byte letti/scritti

    while(true)
    {
        // lettura risposta server e controllo numero di byte letti
        debug("Thread: inizio attesa notifica udp\n");  //debug
        nByte = recv(descrUDP, buf, 3, 0);

        debug("Thread: ricevuto notifica udp, numero byte ricevuto %d\n", nByte);  //debug

        if (nByte != 3)
        {
            // byte letti insufficienti
    
            debug("Thread: lettura risposta server fallita, letti %d\n", nByte); // debug
            debug("Thread: messaggio server:\"%s\"\n", buf);                     // debug
    
            svuota_buffer(descrUDP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo
    
            debug("Thread: buffer svuotato correttamente\n"); // debug
    
            //rilascio risorse
            debug("Thread: inizio rilascio risorse\n"); // debug
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Thread: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Thread: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug
    
            return NOTOK;
        }

        //byte letti corretti

        debug("Thread: numero byte ricevuto corretti\nThread: stampa della notifica corretta in corso\n");  //debug

        if(buf[0] == '0'){
            //ricevuto una richiesta di amicizia
            printf("---NOTIFICA------------------------------------\n");
            printf("- ricevuta richiesta di amicizia\n");
            int numNotifiche = -1;  //DA MODIFICAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
            printf("- hai ancora %d notifiche da leggere\n", numNotifiche);
            printf("-----------------------------------------------\n");

        } else if(buf[0] == '1'){
            //ricevuto una richiesta di amicizia accettata
            printf("---NOTIFICA------------------------------------\n");
            printf("- una richiesta di amicizia è stata accettata\n");
            int numNotifiche = -1;  //DA MODIFICAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
            printf("- hai ancora %d notifiche da leggere\n", numNotifiche);
            printf("-----------------------------------------------\n");

        } else if(buf[0] == '2'){
            //ricevuto una richiesta di amicizia rifiutata
            printf("---NOTIFICA------------------------------------\n");
            printf("- una richiesta di amicizia è stata rifiutata\n");
            int numNotifiche = -1;  //DA MODIFICAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
            printf("-hai ancora %d notifiche da leggere\n", numNotifiche);
            printf("-----------------------------------------------\n");

        } else if(buf[0] == '3'){
            //ricevuto un messaggio di testo
            printf("---NOTIFICA------------------------------------\n");
            printf("- ricevuto messaggio di testo\n");
            int numNotifiche = -1;  //DA MODIFICAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
            printf("-hai ancora %d notifiche da leggere\n", numNotifiche);
            printf("------------------------------------------------\n");

        } else if(buf[0] == '4'){
            //ricevuto un messaggio di tipo flood
            printf("---NOTIFICA------------------------------------\n");
            printf("- ricevuto messaggio di tipo flood\n");
            int numNotifiche = -1;  //DA MODIFICAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
            printf("-hai ancora %d notifiche da leggere\n", numNotifiche);
            printf("-----------------------------------------------\n");

        } else {
            //errore ricezione notifica: protocollo non rispettato

            debug("Thread: ricezione messaggio anomala\n"); // debug

            svuota_buffer(descrUDP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

            debug("Thread: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Thread: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Thread: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }

        debug("Thread: stampa notifica terminata\n");   //debug
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//singal handler usato dal processo padre per sbloccare il thread figlio in attesa notifica
void spegnimentoUDP(int sig) {}

//-------------------INIZIO FUNZIONI "PUBBLICHE"------------------------------------------------------------------------------------------------------------------------

int initialization(const int argc, const char *args[])
{
    //varibile flag
    bool sePortaUdpImpostata = false;

    // inizializzazione variabili globali
    descrTCP = NOTOK;
    descrUDP = NOTOK;
    seDebugAttivo = false;
    seCredenzialiDefault = false;

    //varibile id thread
    tid = 0;

    //inizializzazione utente
    memset(utente.id, 0, 9);
    utente.mdp = 0;
    utente.port = 0;

    //controllo argomenti
    for (unsigned int i = 1; i < argc; i++)
    {
        if (strcmp(args[i], "-d") == 0)
            set_debug(true); // attivazione modalità verbose
        else if (strcmp(args[i], "-c") == 0)
            set_credenzialiDefault(true); // attivazione uso credenziali predefinite
        else if (strcmp(args[i], "-p") == 0)
            {
                sePortaUdpImpostata = true;
                utente.port = atoi(args[i+1]);  //imposta il valore della porta udp
                i++;
            }
        else if (strcmp(args[i], "-?") == 0){
            //stampa menu help e termina
            printf("-d per debug(modalità verbosa)\n-c per credenziali usare di default(credenziali salvate all'interno del codice)\n-p per impostare il valore della porta UDP, non verrà salvata su file\n-? help\n");
            exit(OK);
        }
        else
        {
            // Inserita opzione non riconosciuta

            debug("Client: opzioni non valide, inserito:\"%s\"\n", args[i]);

            return NOTOK;
        }
    }

    if(!sePortaUdpImpostata)
    {
        //verifica porta udp
        debug("Client: verifica porta udp salvata precedentemente\n");  //debug
        FILE* fp = fopen("UDP.options", "r");
        if (fp == NULL)
        {
            //file non esistente o altro errore
            debug("Client: impossibile aprire UDP.options in read\n");  //debug
            fp = fopen("UDP.options", "w"); //tentativo di apertura in write
            if(fp == NULL)
            {
                //errore
                debug("Client: impossibile aprire UDP.options in write, errore\n");  //debug
                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                debug("Client: inizio rilascio risorse\n"); // debug
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            debug("Client: generato seed per rand porta udp\n");  //debug
            //per generazione porta udp
            srand(time(NULL));

            return OK;
        }
        //file esistente
        debug("Client: aperto file UDP.options in read\n");  //debug
        fscanf(fp, "%d", &utente.port);  //lettura porta
        debug("Client: letto da UDP.options:\"%d\"\n", utente.port);  //debug
        fclose(fp); //chiusura fp
        debug("Client: chiuso file UDP.options\n");  //debug         
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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    // connessione riuscita

    debug("Client: connessione riuscita\n"); // debug

    debug("Client: creazione socket UDP\n"); // debug

    // creazione socket udp per comunicazione con server (gestione notifiche)
    struct sockaddr_in socketUDP;                  // allocazione socket
    socketUDP.sin_family = AF_INET;                // socket ipv4

    //verifico porta udp
    if(utente.port == 0){
        //porta non salvata
        utente.port = rand()%10000;
        debug("Client: randomizzata porta UDP: %d\n", utente.port);     //debug
        debug("Client: apertura file UDP.options in write\n");          //debug
        FILE* fp = fopen("UDP.options", "w");

        fprintf(fp, "%d", utente.port);
        debug("Client: scritto su UDP.options: %d\n", utente.port); //debug

        fclose(fp);
        debug("Client: chiuso file UDP.options\n"); //debug
    }

    socketUDP.sin_port = htons(utente.port);          // porta aperta
    socketUDP.sin_addr.s_addr = htons(INADDR_ANY); // indirizzo in ascolto (tutti)

    descrUDP = socket(PF_INET, SOCK_DGRAM, 0); // creazione socket udp ipv4

    debug("Client: generata porta udp %d\n", utente.port); // debug

    // niente connect dato che connessione non affidabile

    debug("Client: creazione socket udp riuscito\n"); // debug

    debug("Client: bind del socket udp\n"); // debug

    r = bind(descrUDP, (struct sockaddr *)&socketUDP, sizeof(struct sockaddr_in)); // bind dato che dobbiamo ricevere notifiche

    if (r == -1) // controllo bind avvenuta con successo
    {
        // bind fallita

        debug("Client: bind del socket udp fallito\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        debug("Client: inizio rilascio risorse\n"); // debug
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // bind riuscita
    debug("Client: bind del socket udp riuscio\n"); // debug

    //creazione thread notifiche UDP
    debug("Client: inizio creazione thread per gestione notifiche UDP\n");  //debug

    if ( pthread_create(&tid, NULL, notificheUDP, NULL) != 0)
    {
        //errore creazione thread
        debug("Client: errore creazione thread per gestione notifiche UDP\n");    //debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        debug("Client: inizio rilascio risorse\n"); // debug
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    //creazione thread notifiche UDP
    debug("Client: fine creazione thread per gestione notifiche UDP\n");    //debug

    return OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int new_client()
{
    debug("Client: Inizio registrazione\n"); // debug

    debug("Client: recupero credenziali\n"); // debug

    if (recupero_credenziali() != OK)
    {
        // errore

        debug("Client: recupero credenziali fallito\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: recupero credenziali riuscito\n"); // debug

    debug("Client: invio messaggio REGIS\n"); // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "REGIS ", 6);
    offset += 6; // Spostiamo offset avanti di 6

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 9, "%-8s", utente.id); // uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset += 8;
    msg[offset] = ' ';
    offset += 1;

    // %04d : usa 5 byte, l'ultimo per il '\0'
    snprintf(msg + offset, 5, "%04d", utente.port);
    offset += 4;
    msg[offset] = ' ';
    offset += 1;

    
    //conversione password in le
    char lit[3];
    usingedIntTOlitEndian(utente.mdp, lit);

    //inseritmento in msg
    msg[offset] = lit[0];
    offset += 1;
    msg[offset] = lit[1];
    offset += 1;

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto diversi da offset

        debug("Client: invio messsaggio REGIS fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "WELCO+++") == 0) // se accetta registrazione
    {
        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: registrazione avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "GOBYE+++") == 0)
    {
        // registrazione rifiutata
        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: registrazione rifiutata con successo\n"); // debug
            return NOTOK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");
        debug("Client: ricezione messaggio anomala\n"); // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // registrazione fallita

    debug("Client: registrazione fallita\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    debug("Client: inizio rilascio risorse\n"); // debug
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int login()
{
    debug("inizio login");

    if (recupero_credenziali() != OK) // per avere dentro struttura user i dati corretti
    {
        debug("Client: recupero credenziali fallito");

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messaggio CONNE\n"); // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "CONNE ", 6);
    offset += 6; // Spostiamo offset avanti di 6

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 9, "%-8s", utente.id); // uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset += 8;
    msg[offset] = ' ';
    offset += 1;

    // %04d : usa 5 byte, l'ultimo per il '\0'
    // snprintf(msg + offset, 5, "%04d", utente.port);
    // offset += 4;
    // msg[offset] = ' ';
    // offset += 1;

    //conversione password in le
    char lit[3];
    usingedIntTOlitEndian(utente.mdp, lit);

    //inseritmento in msg
    msg[offset] = lit[0];
    offset += 1;
    msg[offset] = lit[1];
    offset += 1;

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scrittodiversi da offset

        debug("Client: invio messsaggio CONNE fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "HELLO+++") == 0) // se accetta login
    {

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: login avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "GOBYE+++") == 0)
    {
        // login rifiutata

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: login rifiutata con successo\n"); // debug
            return NOTOK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // login fallita

    debug("Client: login fallita\n");               // debug
    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    debug("Client: inizio rilascio risorse\n"); // debug
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int friend_request(char *requestId)
{
    debug("Clinet: inizio richiesta d'amicizia\n"); // debug
    debug("Client: invio messaggio FRIE?\n");       // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "FRIE? ", 6);
    offset += 6; // Spostiamo offset avanti di 6

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 9, "%-8s", requestId); // uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset += 8;

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto diversi da offset

        debug("Client: invio messsaggio FRIE? fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messsaggio FRIE? riuscito, inviati %d\n", nByte); // debug

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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "FRIE>+++") == 0) // se server trasmette richiesta
    {

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: trasmissione richiesta d'amicizia avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "FRIE<+++") == 0)
    {
        // id sconosciuto

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: trasmissione richiesta d'amicizia rifiutata, id sconosciuto\n"); // debug
            return NOTOK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // trasmissione fallita

    debug("Client: login fallita\n"); // debug

    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    debug("Client: inizio rilascio risorse\n"); // debug
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int send_message(char *idDestination, char *mess)
{
    debug("Clinet: inizio  trasmissione messaggion"); // debug
    debug("Client: invio messaggio MESS?\n");         // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "MESS? ", 6);
    offset += 6; // Spostiamo offset avanti di 6

    // %-8s : scrive la stringa e aggiunge spazi alla fine fino ad arrivare a 8
    snprintf(msg + offset, 10, "%-8s ", idDestination); // uso 9 byte per '\0' dopodichè ci scrivo sopra
    offset += 9;

    // inserisco mess, massimo 200 caratteri compreso '\0'
    memcpy(msg + offset, mess, strlen(mess));
    offset += strlen(mess); // Spostiamo offset avanti di lunghezza mess 
    
    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto minori di offset
        debug("Client: invio messsaggio MESS? fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messsaggio MESS? riuscito, inviati %d\n", nByte); // debug

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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "MESS>+++") == 0) // se server trasmette il messaggio
    {
        // invio riuscito

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: invio messaggio avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(msg, "MESS<+++") == 0)
    {
        // invio fallito

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: invio messaggio rifiutato\n"); // debug
            return NOTOK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // trasmissione fallita

    debug("Client: trasmissione messaggio fallito\n"); // debug

    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    debug("Client: inizio rilascio risorse\n"); // debug
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int flood(char *mess)
{
    debug("Clinet: inizio  trasmissione  flood"); // debug
    debug("Client: invio messaggio FLOO?\n");     // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "FLOO? ", 6);
    offset += 6; // Spostiamo offset avanti di 6

    // inserisco mess, massimo 200 caratteri + '\0'
    snprintf(msg + offset, 201, "%-8s ", mess);
    offset += 200;

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto minori da offset

        debug("Client: invio messsaggio FLOO? fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messsaggio FLOO? riuscito, inviati %d\n", nByte); // debug

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

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "FLOO>+++") == 0) // se server trasmette il messaggio flood
    {
        // invio riuscito

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: invio messaggio flood avvenuta con successo\n"); // debug
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // trasmissione fallita

    debug("Client: trasmissione messaggio fallito\n"); // debug

    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    debug("Client: inizio rilascio risorse\n"); // debug
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int list_client(char *listClient)
{
    debug("Clinet: inizio  richiesta lista client"); // debug
    debug("Client: invio messaggio LIST?\n");        // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "LIST?", 5);
    offset += 5; // Spostiamo offset avanti di 5

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto minori da offset

        debug("Client: invio messsaggio LIST? fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messsaggio LIST? riuscito, inviati %d\n", nByte); // debug

    // byte scritti sufficienti

    debug("Client: lettura risposta server\n"); // debug

    // lettura risposta server e controllo numero di byte letti
    nByte = read(descrTCP, msg, sizeof(char) * DIMBUF);
    if (nByte != 10 && nByte != 11 && nByte != 12)  // ipotizzo che num-item(inviato dal server) possa essere 1,2 byte o 3 in base a se client 100 o >100
    {
        // byte letti errati

        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server fallita, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                     // debug

        //svuota_buffer(descrTCP); non controllo buffer dato che server manda più messaggi di seguito

        //debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    debug("Client: creazione regex per valutazione risposta server\n"); // devug

    regex_t regex; // variabile regex
    char *pattern = "^RLIST (00[1-9]|0[1-9][0-9]|100)\\+\\+\\+$";

    if (regcomp(&regex, pattern, REG_EXTENDED))
    {

        debug("Client: creazione regex fallita\n");

      
        //rilascio risorse
        debug("Client: inizio rilascio risorse\n"); // debug

        regfree(&regex);    //rilascio risorse regex

        seErrore = true;    //errore critico, necessario spegnimento forzato

        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: creazione regex riuscita\n");

    if (regexec(&regex, msg, 0, NULL, 0) != 0)
    {

        // ricezione errata, protocollo non rispettato

        debug("Client: ricezione messaggio anomala, NON RISPETTA REGEX\n"); // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        seErrore = true;    //errore critico, necessario spegnimento forzato

        //rilascio risorse
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // ricezione corretta
    debug("Client: ricezione numero dei client avvenuta con successo\n"); // debug

    // estrazione messaggio
    debug("Client: estraggo il numero di client\n"); // debug

    char *token;

    strtok(msg, " ");
    token = strtok(NULL, "+");

    unsigned int nClient = atoi(token);

    debug("Client: numero client %d\n", nClient); // debug

    // ricezione id client da parte del server

    debug("Client: inizio ricezione id client\n");

    strcpy(listClient, ""); // iniziallizzo

    for (unsigned int i = 0; i < nClient; i++)
    {
        // lettura risposta server e controllo numero di byte letti
        nByte = read(descrTCP, msg, sizeof(char) * 17);
        if (nByte != 17) 
        {
            // byte letti errati

            msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

            debug("Client: lettura risposta server fallita, letti %d\n", nByte); // debug
            debug("Client: messaggio server:\"%s\"\n", msg);                     // debug

            //svuota_buffer(descrTCP); non controllo buffer dato che il server invia vari messaggi di seguito

            debug("Client: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }
        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

        // estrazione messaggio
        debug("Client: estraggo id client\n"); // debug

        strtok(msg, " ");
        token = strtok(NULL, "+");

        debug("Client: id client=\"%s\"\n", token); // debug

        strcat(listClient, token); // aggiunta alla lista dei client
    }

    //controllo buffer strtok
    debug("Client: controllo buffer strtok\n"); //debug

    if(strtok(NULL, " +") != NULL){ //controllo che la stringa sia terminata
        debug("Client: buffer strtok non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        seErrore = true;    //errore critico, necessario spegnimento forzato

        //rilascio risorse
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: controllo buffer strtok terminato con successo\n");  //debug


    debug("Client: terminato ricezione id client\n");

    return OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int read_notify(char *output)
{
    debug("Clinet: inizio  richiesta consultazione\n"); // debug
    debug("Client: invio messaggio CONSU\n");           // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "CONSU", 5);
    offset += 5; // Spostiamo offset avanti di 5

    // aggiungo "+++"
    memcpy(msg + offset, "+++\0", 4);
    offset += 3;

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto minori da != offset

        debug("Client: invio messsaggio CONSU fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: invio messsaggio CONSU riuscito, inviati %d\n", nByte); // debug

    // byte scritti sufficienti

    // lettura risposta server e controllo numero di byte letti
    nByte = read(descrTCP, msg, sizeof(char) * DIMBUF);
    if (nByte < 8 || nByte > 218) // il minimo numero di byte da ricevere è 8 (NOCON+++), il massimo 218 (SSEM>/OOLF>: 5 + " ": 1 + id: 8 + " ": 1 + mess: 200 + "+++": 3 = 218)
    {
        // byte letti errati

        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server fallita, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                     // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    debug("Client: creazione regex per valutazione risposta server\n"); // devug

    regex_t regex;                                                                                                 // variabile regex
    char *pattern = "^((SSEM>|OOLF>) [a-zA-Z0-9]{8} .{1,200}|(EIRF>|FRIEN|NOFRI) [a-zA-Z0-9]{8}|NOCON)\\+\\+\\+$"; // testo regex

    if (regcomp(&regex, pattern, REG_EXTENDED))
    {

        debug("Client: creazione regex fallita\n");

        //rilascio risorse
        regfree(&regex);    //rilascio risorse reeìgex

        seErrore = true;    //errore critico, necessario spegnimento forzato

        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    debug("Client: creazione regex riuscita\n");

    if (regexec(&regex, msg, 0, NULL, 0) != 0)
    {

        // ricezione errata, protocollo non rispettato

        debug("Client: ricezione messaggio anomala, NON RISPETTA REGEX\n"); // debug

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    if (!se_bufferVuoto(descrTCP))
    {
        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }

    // ricezione corretta
    debug("Client: valutazione risposta server successo\n"); // debug

    // estrazione messaggio
    debug("Client: estraggo il tipo di notifica\n"); // debug

    char *tipoNotifica;

    tipoNotifica = strtok(msg, " +");

    debug("Client: tipo di notifica: %s\n", tipoNotifica);

    if (strcmp(tipoNotifica, "SSEM>") == 0)
    { // se server trasmette un flusso di tipo messaggio client
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id e il testo del messaggio

            debug("Client: tipo di notifica = messaggio, estraggo id del mittente e il testo del messaggio\n");

            char *id;
            char *mess;

            id = strtok(NULL, " ");
            mess = strtok(NULL, "+");

            debug("Client: id:\"%s\", mess:\"%s\"\n", id, mess);

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer strtok non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto messaggio da\"%s\", testo:\"%s\"\n", id, mess);

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(tipoNotifica, "OOLF>") == 0)
    { // se server trasmette un flusso di tipo inondazione FLOO
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id e il testo del messaggio

            debug("Client: tipo di notifica = inondazione, estraggo id del mittente e il testo del messaggio\n");

            char *id;
            char *mess;

            id = strtok(NULL, " ");
            mess = strtok(NULL, "+");

            debug("Client: id:\"%s\", mess:\"%s\"\n", id, mess);

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto messaggio di inondazione da\"%s\", testo:\"%s\"\n", id, mess);

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(tipoNotifica, "EIRF>") == 0)
    { // se server trasmette un flusso di tipo richiesta d'amicizia
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id del client a cui ho inviato una richiesta d'amicizia

            debug("Client: tipo di notifica = richiesta d'amicizia, estraggo id del mittente\n");

            char *id;

            id = strtok(NULL, "+");

            debug("Client: id:\"%s\"\n", id);

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto richiesta di amicizia da \"%s\"\n", id);

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return FRIEND_REQUEST;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(tipoNotifica, "FRIEN") == 0)
    { // se server trasmette un flusso di tipo risposta a richiesta d'amicizia accept
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id del client che ha risposto  alla mia richiesta d'amicizia

            debug("Client: tipo di notifica = risposta a richiesta d'amicizia, estraggo id del mittente\n");

            char *id;

            id = strtok(NULL, "+");

            debug("Client: id:\"%s\"\n", id);

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto richiesta di amicizia accettata da \"%s\"\n", id);

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(tipoNotifica, "NOFRI") == 0)
    { // se server trasmette un flusso di tipo risposta a richiesta d'amicizia deny
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id del client che ha risposto  alla mia richiesta d'amicizia

            debug("Client: tipo di notifica = risposta a richiesta d'amicizia, estraggo id del mittente\n");

            char *id;

            id = strtok(NULL, "+");

            debug("Client: id:\"%s\"\n", id);

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto richiesta di amicizia rifiutata da \"%s\"\n", id);

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    else if (strcmp(tipoNotifica, "NOCON") == 0)
    { // se server trasmette un flusso di tipo terminati flussi da consultare
        // ricezione messaggio

        if (se_bufferVuoto(descrTCP))
        {
            // estraggo id del client che ha risposto  alla mia richiesta d'amicizia

            debug("Client: tipo di notifica = terminati flussi da consultare\n");

            if (strtok(NULL, " +") != NULL)
            { // controllo che la stringa sia terminata
                debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

                svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

                debug("Client: buffer svuotato correttamente\n"); // debug

                //rilascio risorse
                seErrore = true;    //errore critico, necessario spegnimento forzato
                if(client_shutdown() == OK)
                    debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
                else
                    debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

                return NOTOK;
            }

            // creazione stringa output
            sprintf(output, "Ricevuto terminate notifiche\n");

            debug("Client: creazione stringa output avvenuta con successo\n"); // debug

            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
    }
    // consultazione fallita

    debug("Client: consultazione notifiche fallita\n"); // debug

    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    //rilascio risorse
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int friend_request_response(char response)
{
    debug("Client: inizio  risposta a richiesta d'amicizia\n"); // debug

    debug("Client: valutazione risposta utente\n");

    switch (response)
    {
    case 'y':
    {
        debug("Client: valutato accettazione richiesta d'amicizia\n");

        // creo buffer e riempo con messaggio finale
        char msg[DIMBUF];
        int offset = 0;

        memcpy(msg + offset, "OKIRF", 5);
        offset += 5; // Spostiamo offset avanti di 5

        // aggiungo "+++"
        memcpy(msg + offset, "+++\0", 4);
        offset += 3;

        debug("Client: messaggio:\"%s\"\n", msg); // debug

        int nByte = 0; // contatore byte letti/scritti

        nByte = write(descrTCP, msg, offset);
        if (nByte != offset) // controllo numero byte scritti
        {
            // byte scritto minori da offset

            debug("Client: invio messsaggio OKIRF fallito, scritti %d\n", nByte); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }

        debug("Client: invio messsaggio OKIRF riuscito, inviati %d\n", nByte); // debug

        // byte scritti sufficienti

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

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }
        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

        if (strcmp(msg, "ACKRF+++") == 0) // se server riceve correttamente
        {
            // invio riuscito

            if (se_bufferVuoto(descrTCP))
            {
                debug("Client: invio accettazione richiesta d'amicizia avvenuta con successo\n"); // debug
                return OK;
            }

            debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

            svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

            debug("Client: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }
        else
        {
            debug("Client: invio accettazione richiesta d'amicizia fallita\n"); // debug

            svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

            debug("Client: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }

        break;
    }
    case 'n':
    {
        debug("Client: valutato rifiutare richiesta d'amicizia\n");

        // creo buffer e riempo con messaggio finale
        char msg[DIMBUF];
        int offset = 0;

        memcpy(msg + offset, "NOIRF", 5);
        offset += 5; // Spostiamo offset avanti di 5

        // aggiungo "+++"
        memcpy(msg + offset, "+++\0", 4);
        offset += 3;

        debug("Client: messaggio:\"%s\"\n", msg); // debug

        int nByte = 0; // contatore byte letti/scritti

        nByte = write(descrTCP, msg, offset);
        if (nByte != offset) // controllo numero byte scritti
        {
            // byte scritto minori da offset

            debug("Client: invio messsaggio NOIRF fallito, scritti %d\n", nByte); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }

        debug("Client: invio messsaggio NOIRF riuscito, inviati %d\n", nByte); // debug

        // byte scritti sufficienti

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

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }
        msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

        debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
        debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

        if (strcmp(msg, "ACKRF+++") == 0) // se server riceve correttamente
        {
            // invio riuscito

            if (se_bufferVuoto(descrTCP))
            {
                debug("Client: invio rifiuto richiesta d'amicizia avvenuta con successo\n"); // debug
                return OK;
            }

            debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

            svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

            debug("Client: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }
        else
        {
            debug("Client: invio rifiuto richiesta d'amicizia fallita\n"); // debug

            svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

            debug("Client: buffer svuotato correttamente\n"); // debug

            //rilascio risorse
            seErrore = true;    //errore critico, necessario spegnimento forzato
            if(client_shutdown() == OK)
                debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
            else
                debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

            return NOTOK;
        }

        break;
    }
    default:
    {
        debug("Client: parametro responde non riconosciuto\n");                  // debug
        debug("Client: trasmissione risposta a richiesta d'amicizia fallita\n"); // debug

        //rilascio risorse
        seErrore = true;    //errore critico, necessario spegnimento forzato
        if(client_shutdown() == OK)
            debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
        else
            debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

        return NOTOK;
        break;
    }
    }

    debug("Client: parametro responde non riconosciuto\n");                  // debug
    debug("Client: trasmissione risposta a richiesta d'amicizia fallita\n"); // debug

    //rilascio risorse
    seErrore = true;    //errore critico, necessario spegnimento forzato
    if(client_shutdown() == OK)
        debug("Client: fine rilascio risorse rilascio risorse terminato con successo\n"); // debug
    else
        debug("Client: fine rilascio risorse rilascio risorse terminato con esito negativo\n"); // debug

    return NOTOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int client_shutdown()
{
    debug("Client: inizio spegnimento e disconnessione\n"); // debug

    // if(tid != 0)
    // {
    //     //spegnimento socket udp in uso dal thread
    //     debug("Client: spegnimento socket UDP inizio\n");   //debug

    //     // int rit = shutdown(descrUDP, SHUT_RDWR);

    //     // if ( rit != 0)
    //     //     debug("Client: spegnimento socket UDP fallita, valore di ritorno %d\n", rit);  //debug
    //     // else
    //     //     debug("Client: spegnimento socket UDP riuscita\n");  //debug

        

    //     //spegnimento thread
    //     debug("Client: inizio spegnimento thread\n");   //debug
    
        
    //     pthread_join(tid, NULL);
    
    //     debug("Client: fine spegnimento thread\n"); //debug
    // }

    pthread_kill(tid, SIGUSR1); //mando segnale di kill al thread

    debug("Client: invio messaggio IQUIT\n");               // debug

    // creo buffer e riempo con messaggio finale
    char msg[DIMBUF];
    int offset = 0;

    memcpy(msg + offset, "IQUIT+++\0", 9);
    offset += 8; // Spostiamo offset avanti di 8

    debug("Client: messaggio:\"%s\"\n", msg); // debug

    int nByte = 0; // contatore byte letti/scritti

    nByte = write(descrTCP, msg, offset);
    if (nByte != offset) // controllo numero byte scritti
    {
        // byte scritto minori da offset

        debug("Client: invio messsaggio IQUIT fallito, scritti %d\n", nByte); // debug

        //rilascio risorse
        close(descrTCP);
        close(descrUDP);

        //controllo se errore critico
        if(seErrore)
            exit(NOTOK);

        return NOTOK;
    }

    debug("Client: invio messsaggio IQUIT riuscito, inviati %d\n", nByte); // debug

    // byte scritti sufficienti

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

        //rilascio risorse
        close(descrTCP);
        close(descrUDP);
        
        //controllo se errore critico
        if(seErrore)
            exit(NOTOK);   

        return NOTOK;
    }
    msg[nByte] = '\0'; // imposto fine stringa per evitare di leggere caratteri di messaggi precedenti

    debug("Client: lettura risposta server riuscito, letti %d\n", nByte); // debug
    debug("Client: messaggio server:\"%s\"\n", msg);                      // debug

    if (strcmp(msg, "GOBYE+++") == 0) // se server "accetta" disconnessione
    {

        if (se_bufferVuoto(descrTCP))
        {
            debug("Client: spegnimento e disconnessione avvenuta con successo\n"); // debug

            //rilascio risorse
            debug("Client: chiusura socket TCP\n"); //debug
            close(descrTCP);
            debug("Client: chiusura socket UDP\n");
            close(descrUDP);

            //controllo se errore critico
            debug("Client: errore critico: %d", seErrore);
                exit(NOTOK);
     
            return OK;
        }

        debug("Client: buffer non vuoto dopo lettura messaggio, protocollo non rispettato\n");

        svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

        debug("Client: buffer svuotato correttamente\n"); // debug

        //rilascio risorse
        close(descrTCP);
        close(descrUDP);

        //controllo se errore critico
        if(seErrore)
            exit(NOTOK);

        return NOTOK;
    }

    // trasmissione fallita

    debug("Client: disconessione fallita\n"); // debug

    debug("Client: ricezione messaggio anomala\n"); // debug

    svuota_buffer(descrTCP); // svuoto buffer dato che il messaggio ricevuto non rispetta il protocollo

    debug("Client: buffer svuotato correttamente\n"); // debug

    close(descrTCP);
    close(descrUDP);

    //controllo se errore critico
    if(seErrore)
        exit(NOTOK);
    

    return NOTOK;
}
