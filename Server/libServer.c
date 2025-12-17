#include "Server.h"
#include "../src/utility.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>


/*-------Variabili & Semafori-------*/

//-------Debug
unsigned int DEB = 0;

//-------Gestione User
static user listUser [MAX_CLIENT];
static unsigned int nUser = 0;
pthread_mutex_t semList = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semNuser = PTHREAD_MUTEX_INITIALIZER;


//-------Gestione UDP
int udpSocket = -1;
pthread_mutex_t semUDP = PTHREAD_MUTEX_INITIALIZER;

/*------------------------------------*/


/*-------------------Funzioni Gestione Utenti-------------------*/

int findUser(const char id [ID_LEN]){
    pthread_mutex_lock(&semNuser);
    if(nUser == 0) {
        pthread_mutex_unlock(&semNuser); 
        return NOTFIND;
    }
    pthread_mutex_lock (&semList); 

    
    for(unsigned int i = 0; i<nUser; i++){

        if(strcmp(id, listUser[i].id) == 0){
            pthread_mutex_unlock(&semNuser);
            pthread_mutex_unlock(&semList);
            return i;
        }

    }   

    pthread_mutex_unlock(&semNuser);
    pthread_mutex_unlock(&semList);
    return NOTFIND;

}

/*unsigned int modUser (const unsigned int pox, struct sockaddr_in add){

    if(pox < 0) return NOTOK;

    pthread_mutex_lock(&semNuser);
    if(nUser < 1) {
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    } 
    pthread_mutex_unlock(&semNuser);


    pthread_mutex_lock(&semList);

    listUser[pox].add = add;

    pthread_mutex_lock(&semList);
    

    return OK;

}*/

unsigned int addUser(const char id [ID_LEN], struct sockaddr_in add, unsigned int pass){
    // int pox;
    // if((pox = findUser(id)) != NOTFIND) return modUser(pox, add); //potrebbe non aver senso 
    
    if(DEB) printf("Avvio funzione addUser \n");

    //se gia presente non lo fa connettere
    if(findUser(id) != NOTFIND) return NOTOK;   

    pthread_mutex_lock(&semNuser);

    //IL SERVER è PIENO 
    if(nUser >= MAX_CLIENT){
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    }

    user newUser;
    strcpy(newUser.id, id);
    newUser.add = add;
    newUser.pass = pass;
    newUser.nMsg = 0;
    newUser.listMsg = NULL;
    newUser.nFri = 0;

    pthread_mutex_lock (&semList); 


    listUser[nUser] = newUser;
    nUser ++;

    pthread_mutex_unlock(&semNuser);
    pthread_mutex_unlock(&semList);


    return OK;
}

//devo controllare che idR (idReciver) è amico di idS (idSender)
//dato che solo se si è amici allora si può inviare un messaggio 
unsigned int friendAS (const char idR[ID_LEN], const char idS [ID_LEN]){
    pthread_mutex_lock(&semNuser);
    if(nUser <= 1) {
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    } 
    pthread_mutex_unlock(&semNuser);

    
    //non c'è bisogno di semafori prima in quando in findUser vi sono 
    unsigned int pox;
    //Controllo che idS e idR esistano, e se si mi salvo la poszioned di idR
    if( findUser (idS) == NOTFIND || (pox = findUser(idR)) == NOTFIND ) return NOTOK;
    

    //-----Controllo che siano amici 
    pthread_mutex_lock(&semList);

    user user_idR = listUser[pox];

    for(unsigned int i = 0; i< user_idR.nFri; i++){
        if(strcmp(user_idR.listFri[i], idS) == 0){
            pthread_mutex_unlock(&semList);
            return OK;
        }
    }

    pthread_mutex_unlock(&semList);
    return NOTOK;

}

//devo aggiungere ad idD (idDestination), idS (idSorgente) come amico; 
unsigned int addFrined (const char idD [ID_LEN], const char idS [ID_LEN]){
    if(friendAS(idD, idS) == OK){
        if(DEB) printf("addFrined: gli utenti sono già amaici\n");
        return OK;
    }

    pthread_mutex_lock(&semNuser);
    if(nUser <= 1) {
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    } 
    pthread_mutex_unlock(&semNuser);

    //non c'è bisogno di semafori prima in quando in findUser vi sono 
    unsigned int pox;
    //Controllo che idS e idR esistano, e se si mi salvo la poszioned di idR
    if( findUser (idS) == NOTFIND || (pox =findUser(idD)) == NOTFIND ) return NOTOK;
    
   //-----Aggiungo amico  
    pthread_mutex_lock(&semList);
    unsigned int nFreind = listUser[pox].nFri;
    if(nFreind >= MAX_CLIENT-1){
        pthread_mutex_unlock(&semList);
        return NOTOK;
    }

    strcpy(listUser[pox].listFri[nFreind], idS);
    listUser[pox].nFri ++;
    pthread_mutex_unlock(&semList);


    return addFrined(idS, idD); // se user accetta l'amicizia allora va aggiunto a tutti e due 
    
}


//aggiungo ad idD (idDestinatario) il messaggio che arriva da idS (idSorgente)
unsigned int addMSG (const char idD [ID_LEN], const char idS[ID_LEN], const char value[MAX_LEN], typFlux tip){
    pthread_mutex_lock(&semNuser);
    if(nUser <1) {
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    } 
    pthread_mutex_unlock(&semNuser);

    //non c'è bisogno di semafori prima in quando in findUser vi sono 
    unsigned int pox;
    //Controllo che idS e idR esistano, e se si mi salvo la poszioned di idR
    if( findUser (idS) == NOTFIND || (pox =findUser(idD)) == NOTFIND ) return NOTOK;

    //Creazione messaggio 
    msg * newMsg  = (msg* ) malloc (sizeof(msg));
    if(newMsg == NULL) return NOTOK;
    strcpy(newMsg->id, idS);
    if(tip == MSG  || tip == FLOO){
        strcpy(newMsg->value, value);
    }
    newMsg->typeMSG = tip;
    newMsg->next = NULL;

    pthread_mutex_lock(&semNuser);




    if(listUser[pox].listMsg == NULL){
        listUser[pox].listMsg = newMsg;
        listUser[pox].nMsg++;
       
        
        pthread_mutex_unlock(&semNuser);
        return OK;
    }

    msg * currMsg = listUser[pox].listMsg;

    while (currMsg->next != NULL){
        currMsg = currMsg->next;
    }

    currMsg->next = newMsg;
    
    listUser[pox].nMsg++;
    
   

    pthread_mutex_unlock(&semNuser);



    return OK;
}

//rimuove solo il il primo messaggio, dato che quando si fa consult, si ritorna solo il primo messaggio
//oltre a rimuovere mi da anche il valore del primo messaggio, inserito al interno di msgRM -> messaggio rimosso  
/*
    NOTFIND -> utente non trovato
    NOTOK -> non ha flussi
*/
int rmMSG(const char idD [ID_LEN], msg *msgRM){
    unsigned int pox;
    if((pox = findUser(idD)) == NOTFIND) return NOTFIND;

    pthread_mutex_lock(&semList);

    if( listUser [pox].nMsg == 0) {
        listUser[pox].listMsg = NULL;
        pthread_mutex_unlock(&semList);
        return NOTOK;
    }

    msgRM = listUser[pox].listMsg;
    listUser[pox].listMsg = msgRM->next;

    listUser[pox].nMsg--;

    pthread_mutex_unlock(&semList);

    return OK;
}

/*--------------------------------------------------------------*/


/*------------------Funzioni Gestione Utility------------------*/


unsigned int okMSG(const char * msg, size_t dimMsg){
    if(dimMsg<8){
        return NOTOK;
    }

    
   if(DEB) printf("Controllo che il messaggio inviato sia coretto\n");

    dimMsg--;

    if (dimMsg >= 3) {
        if (msg[dimMsg - 2] != '+' && 
            msg[dimMsg - 1] != '+' && 
            msg[dimMsg] != '+') {
            return NOTOK;
        }
    }

    char type[6];
     
    strncpy(type, msg, 5);

    type[5] = '\0';

    if(DEB) printf("il valore dei primi 5 byte e: %s\n", type);

    return OK;
}


/*--------------------------------------------------------------*/




/*-------------------Funzioni TCP e UDP-------------------*/

unsigned int serverInit (unsigned int d){
    if(d){
        printf ("Server Start Modalità debug:\n");
        DEB = 1;
    }
    else{
        printf ("Server Start:\n");
    }

    if(pthread_mutex_init(&semList, NULL) != 0 || 
        pthread_mutex_init(&semNuser, NULL) != 0  || 
            pthread_mutex_init(&semUDP, NULL) != 0 
    ){

        perror("server init");
        return NOTOK;
    }

    pthread_mutex_unlock(&semList);
    pthread_mutex_unlock(&semNuser);
    pthread_mutex_unlock(&semUDP);


    //Creazione socket UDP
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) {
        perror("UDP socket creation");
        return NOTOK;
    }

    struct sockaddr_in udp_addr;
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(0); 
      
    if(bind(udpSocket, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("UDP bind");
        close(udpSocket);
        return NOTOK;
    }

    return OK;
}

void serverClose() {
    if(udpSocket >= 0) {
        close(udpSocket);
    }
    pthread_mutex_destroy(&semList);
    pthread_mutex_destroy(&semNuser);
    pthread_mutex_destroy(&semUDP);

    printf("Server close\n");
}

unsigned int simpleTCPmsg (int sock, typSimpleMsg tip){
    char msg [9];
 switch (tip){
        case WELCO: 
            strcpy(msg, "WELCO+++\0");
            break; 
        case GOBYE: 
            strcpy(msg, "GOBYE+++\0");
            break; 
        case HELLO: 
            strcpy(msg, "HELLO+++\0");
            break; 
        case FRIE_OK: 
            strcpy(msg, "FRIE>+++\0");
            break; 
        case FRIE_NOTOK: 
            strcpy(msg, "FRIE<+++\0");
            break; 
        case MESS_OK: 
            strcpy(msg, "MESS>+++\0");
            break; 
        case MESS_NOTOK: 
            strcpy(msg, "MESS<+++\0");
            break; 
        case FLOO_OK: 
            strcpy(msg, "FLOO>+++\0");
            break; 
        case ACKRF: 
            strcpy(msg, "ACKRF+++\0");
            break; 
        case NOCON: 
            strcpy(msg, "NOCON+++\0");
            break; 
        default:
            return NOTOK; // Questo è corretto per i tipi non riconosciuti
    }
    ssize_t byteSent = write(sock, msg, strlen(msg)*sizeof(char));  

    if(byteSent <= 0) return NOTOK;

    return OK;

}

unsigned int sendUDPmessage(const char IDR[ID_LEN], typFlux tip){
    unsigned int pox;
    if((pox = findUser(IDR)) == NOTFIND) return NOTOK;

    char typeChar;

    switch (tip){
    case FRIE_Req:
        typeChar = '0';
        break;
    
    case FRIE_A:
        typeChar = '1';
        break;

    case FRIE_R:
        typeChar = '2';
        break;

    case MSG:
        typeChar = '3';
        break;

    case FLOO:
        typeChar = '4';
        break;    
    default:
        if(DEB) printf("sendUDPmessage: TYPE NOT EXIST\n");
        return NOTOK;
    }

    char numberLittelEndian [3];

    pthread_mutex_lock(&semList);
    
    usingedIntTOlitEndian(listUser[pox].nMsg, numberLittelEndian);

    pthread_mutex_unlock(&semList);

    //if(DEB) printf("numberLittelEndian[0]: %d\nnumberLittelEndian[1] %d\n", numberLittelEndian[0], numberLittelEndian[1]);


    unsigned char mess[4];

    mess[0] = typeChar;
    mess[1] = numberLittelEndian[0];
    mess[2] = numberLittelEndian[1];
    mess[3] =  '\0';
    
    pthread_mutex_lock(&semList);

    struct sockaddr_in addrReciver = listUser[pox].add;
    
    pthread_mutex_unlock(&semList);
    

    if(DEB){
        char *client_ip = inet_ntoa(addrReciver.sin_addr);
        printf("sendUDPmessage: invio di messaggio a:\n");
        printf("    IP: %s\n", client_ip);
        printf("    PORT: %u\n", htons(addrReciver.sin_port));
        printf("    ID: %s\n", IDR);
    }


    pthread_mutex_lock(&semUDP);

    ssize_t byteSend = sendto(udpSocket, mess, 3,0, (struct sockaddr *) &addrReciver, sizeof(addrReciver));

    pthread_mutex_unlock(&semUDP);

    if(byteSend < 0) return NOTOK;

    if(DEB) printf("sendUDPmessage: notfication send: [ %s ]\n", mess);

    return OK;

}

//funziona ma insicura !!!
unsigned int readTCPmessage (int sock, char * buff, size_t dimBuff){
    if (buff == NULL || dimBuff < 8) {
        return NOTOK;
    }
    
    size_t msgTotlen = 0;
    unsigned int plus = 0;
    char tempBuf;
    
    
    // Inizializza il buffer
    memset(buff, 0, dimBuff);

    
    
    // Legge byte per byte fino a trovare "+++"
    while (msgTotlen < dimBuff - 1) {
        ssize_t bytesRead = read(sock, &tempBuf, 1);
        //if(DEB) printf("byte letto: %c\n", tempBuf);
        
        if (bytesRead <= 0) {
            // Errore o connessione chiusa
            return NOTOK;
        }

        
        
        if(isalnum(tempBuf) || tempBuf == '+' || tempBuf == ' ' || tempBuf == '?' || tempBuf == '<' || tempBuf == '>'){
            //if(DEB) printf("byte scritto: %c\n", tempBuf);

            buff[msgTotlen] = tempBuf;
            msgTotlen++;
            
            // Controlla se abbiamo trovato la sequenza terminatrice "+++"
            if (tempBuf == '+') {
                plus++;
                if (plus == 3) {
                    break; 
                }
            } else {
                plus = 0; 
        }
        }
       
    }

    if(msgTotlen <8) return NOTOK;

    buff[msgTotlen] ='\0';
    return OK;


}

unsigned int REGIST(const char *msg, struct sockaddr_in TCP_ADDR_Client){

    //minima lunghezza:  REGIS (5) + (1) + ID(8) + (1) + PORT (4) +(1) + mdp (2) +"+++" (3) = 25
    if(strlen(msg) < 25){
        if(DEB) printf("REGIS: messaggio troppo corto\n");
        return NOTOK;
    }

    if(strncmp(msg, "REGIS ", 6) != 0) {
        if(DEB) printf("REGIS: formato non valido\n");
        return NOTOK;
    }

    char id[ID_LEN];
    char strPORT [5]; // 4 caratteri + /0
    unsigned char mdpBYTE[2]; //i due byte della pass


    strncpy(id, msg+6, (ID_LEN-1)); //sto copiando solo l'id --> REGIS (5) + (1) = 6 byte da saltare 
    id[(ID_LEN-1)] ='\0';

    strncpy(strPORT, msg+15, 4); //sto copiando solo port -->  REGIS (5) + (1) + ID(8) + (1) = 15 byte da slatare
    strPORT[4] ='\0';

    //estraggo solo i due byte del mdp --> non metto terminazione 
    mdpBYTE[0] = (unsigned char) msg[20]; //primo byte 
    mdpBYTE[1] = (unsigned char) msg[21]; //secondo byte 


    // Converte la password da little-endian a unsigned int
    unsigned int pass = litEndianTOusingedInt(mdpBYTE);
    

    unsigned int portUDP = atoi(strPORT);

    /*--------VALIDAZIONE DEI DATI--------*/

    if(pass > 65535){
        if(DEB) printf("REGIS: pass maggiore di 65535\n");
        return NOTOK;
    }

    if(portUDP >= 9999){
        if(DEB) printf("REGIS: UDP port maggiore o uguale a 9999\n");
        return NOTOK;
    }

    /*
        for(int i = 0; i < 8; i++) {
            if(!isalnum(id[i])) {
                if(DEB) printf("REGIS: ID contiene caratteri non alfanumerici\n");
                simpleTCPmsg(sock, GOBYE);
                return NOTOK;
            }
        }
    
    */

    /*------------------------------------*/


    //-----Costruzione l'indirizzo UDP client
    struct sockaddr_in UDP_ADDR_Client;
    UDP_ADDR_Client.sin_family =AF_INET;
    UDP_ADDR_Client.sin_addr = TCP_ADDR_Client.sin_addr;
    UDP_ADDR_Client.sin_port = htons(portUDP);


    if(DEB){
        char *client_ip = inet_ntoa(UDP_ADDR_Client.sin_addr);
        printf("REGIST: tentativo di registrazione su list\n");
        printf("    IP: %s\n", client_ip);
        printf("    PORT: %u\n", portUDP);
        printf("    ID: %s\n", id);
    }

    if(addUser(id, UDP_ADDR_Client, pass) == NOTOK){
        if(DEB) printf("REGIS: impossibile aggiungere utente (server pieno o già esistente)\n");
        return NOTOK;
    }

    

    if(DEB) printf("REGIS: utente %s registrato con successo\n", id);
    return OK;

}

unsigned int CONNECT(const char *msg){

    //conferma solo se l'utente esisete già e se la pass è corretta

    //controlla che ci sia almeno un utente
    pthread_mutex_lock(&semNuser);
    if(nUser <= 0){
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    }
    pthread_mutex_unlock(&semNuser);


    /*
        lunghezza minima:
            CONNE (5) + (1) + ID(8) + (1) + PASS (2) + "+++" (3) = 20 byte
    
    */
    if(strlen(msg) < 20){
        if(DEB) printf("CONNECT: messaggio troppo corto\n");
        return NOTOK;
    }

    if(strncmp(msg, "CONNE ", 6) != 0) {
        if(DEB) printf("CONNECT: formato non valido\n");
        return NOTOK;
    }

    

    char id [ID_LEN];
    unsigned char mdpBYTE[2];

    strncpy(id, msg+6, (ID_LEN-1));
    id[(ID_LEN-1)] = '\0';

    unsigned int pox;
    if((pox = findUser(id)) == NOTFIND){
        if(DEB) printf("CONNECT: utente non trovato\n");
        return NOTOK;
    }


    mdpBYTE[0] = (unsigned char) msg[15];
    mdpBYTE[1] = (unsigned char) msg[16];

    unsigned int pass = litEndianTOusingedInt(mdpBYTE);

    pthread_mutex_lock(&semList);

    if(listUser[pox].pass != pass){
        if(DEB) printf("CONNECT: password non corretta\n   Pass in database: %u\n   Pass in avente: %u", listUser[pox].pass, pass);
        pthread_mutex_unlock(&semList);
        return NOTOK;
    }

    pthread_mutex_unlock(&semList);

    if(DEB) printf("CONNECT: utente %s connesso con successo\n", id);

    return OK;

}

unsigned int MESSAGE(const char *msg, const char idSender[ID_LEN]){
    //controllo che gli utenti siano amici e che esistano 

    if(findUser(idSender) == NOTFIND){
        if(DEB) printf("MESSAGE: User sendere not fund\n");
        return NOTOK;
    }

    //controllo che ci siano alemno due user 
    pthread_mutex_lock(&semNuser);
    if(nUser <= 1){
        if(DEB) printf("MESSAGE: non ci sono abbastanza persone\n");
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    }
    pthread_mutex_unlock(&semNuser);


    /*
        lunghezza masssima:
            MESS? (5) + (1) + ID(8) + (1) + MESS(200) + "+++" (3) = 218 byte

        lunghezza minima:
            MESS? (5) + (1) + ID(8) + (1) + MESS(1) + "+++" (3) = 11 byte

    */
    size_t sizeStr  = strlen(msg);

    //if(DEB) printf("il messaggio: %s è lungo %lu byte\n", msg, sizeStr);

    if(sizeStr<6 || sizeStr>218){
        if(DEB) printf("MESSAGE: messaggio non coretto\n");
        return NOTOK;
    }

    char IDR[ID_LEN];

    strncpy(IDR, msg+6, (ID_LEN-1));
    IDR[(ID_LEN-1)] = '\0';

    char contMSG [MAX_LEN];
    size_t byteMSG = (strlen(msg) - 18); 

    strncpy(contMSG, msg+15, byteMSG);

    if(DEB) printf("il messaggio da inviare \"%s\"  ed è lungo %lu byte\n", contMSG,byteMSG);

    if(sendUDPmessage(IDR, MSG) == NOTOK){
        if(DEB) printf("MESSAGE: la notifica non è stata inviata e quindi non è stato aggiunto il messaggio\n");
        return NOTOK;
    }

    
    if(addMSG(IDR, idSender, contMSG, MSG) == NOTOK){
        if(DEB) printf("MESSAGE: il messaggio non è stato aggiunto alla lista di %s\n", IDR);
        return NOTOK;
    }

    return OK;

}

unsigned int FREIREQ(const char *msg, const char idSender[ID_LEN]){
    pthread_mutex_lock(&semNuser);
    if(nUser <= 1){
        if(DEB) printf("FREIREQ: non ci sono abbastanza persone\n");
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    }
    pthread_mutex_unlock(&semNuser);

    /*
        lunghezza: 
            FRIE? (5) + (1) + ID (8) + "+++" (3) = 17 byte

    */

    if(strlen(msg) != 17){
        if(DEB) printf("FREIREQ: Lunghezza del messaggio non coretta\n");
        return NOTOK;
    }

    if(strncmp(msg, "FRIE? ", 6) != 0){
        if(DEB) printf("FREIREQ: Messaggio non coretto\n");
        return NOTOK;
    }


    char IDR [ID_LEN];

    strncpy(IDR, msg+6, (ID_LEN-1));
    IDR[(ID_LEN-1)] = '\0';

    if(findUser(IDR) == NOTFIND){
        if(DEB) printf("FREIREQ: User [ %s ] non trovato\n", IDR);
        return NOTOK;
    }

    if(sendUDPmessage(IDR, FRIE_Req) == NOTOK ){
        if(DEB) printf("FREIREQ: notifica udp non avvenuta, non aggiungo alla lista\n");
        return NOTOK;
    }

    if(addMSG(IDR, idSender, NULL, FRIE_Req) == NOTFIND){
        if(DEB) printf("FREIREQ: richiesta non aggiunta\n");
        return NOTOK;
    }

   return OK;
}

/*==========LIST==========*/

unsigned int LISTNUM(int sock){
    //cosidero che almeno un user ci sia;

    /*
        [13] -> RLIST (5) + (1) + numItem (3 -> 100) + "+++" (3) + '\0' (1)
    */
    char msgSend [13];

    pthread_mutex_lock(&semNuser);

    sprintf(msgSend, "RLIST %u+++", nUser);

    pthread_mutex_unlock(&semNuser);


    if(DEB) printf("LISTNUM: invia il messaggio \"%s\"\n", msgSend);

    ssize_t byteSent = write(sock, msgSend, strlen(msgSend)*sizeof(char));  

    if(byteSent <= 0) return NOTOK;

    return OK;
    
}

unsigned int LISTUSER(int sock){
    
    //considero che ci sia almeno un utente;

    /*
    
        [18] = LINUM (5) +(1) + ID (8) + "+++" (3) + '\0' (1)

    */
    char msgSend[18];

    pthread_mutex_lock(&semNuser);
    pthread_mutex_lock(&semList);

    for(unsigned int i = 0; i < nUser; i++){
        sprintf(msgSend, "LINUM %s+++", listUser[i].id);

        ssize_t byteSent = write(sock, msgSend, strlen(msgSend)*sizeof(char));  

        if(byteSent <= 0){
            pthread_mutex_unlock(&semNuser);
            pthread_mutex_unlock(&semList);
            return NOTOK;
        }

        if(DEB) printf("LISTUSER: invia il messaggio \"%s\"\n", msgSend);

    }


    pthread_mutex_unlock(&semNuser);
    pthread_mutex_unlock(&semList);

    return OK;
}

/*========================*/


/*==========CONSU==========*/

unsigned int CONUSLT (int sock, const char ID [ID_LEN]){
    msg Flux;

    if(rmMSG()){

    }

}

/*=========================*/



/*-----------------------------------------------------*/


/*------------------Funzione per Thread------------------*/



void * pthreadConection(void * sockClient){
    int sClient =  * (int *) sockClient;
    //libero memoria allocata, nel main per librerare spazio 
    //dato la possibilià che vi siano più client 
    free(sockClient);

    unsigned int connesso = 1;
    unsigned int ok = 1;


    printf("\n=== Thread avviato per socket %d ===\n", sClient);


    struct sockaddr_in client_tcp_addr;
    socklen_t addr_len = sizeof(client_tcp_addr);
    if(getpeername(sClient, (struct sockaddr*)&client_tcp_addr, &addr_len) < 0) {
        perror("getpeername");
        close(sClient);
        pthread_exit(NULL);
    }

    if(DEB){
        char *client_ip = inet_ntoa(client_tcp_addr.sin_addr);
        printf("Thread si è connesso con: %s\n", client_ip);
    } 


    char buff[MAX_TCP_MESAGGE];
    char id [ID_LEN];
    
    if(readTCPmessage(sClient, buff, sizeof(buff)) == NOTOK){
        if(DEB) printf("il messaggio letto non è ok:  CHIUDO LA CONNESIONE\n");
        ok = 0;
    }

    char type[6];
    strncpy(type, buff, 5);
    type[5]='\0';

    if(strcmp(type, "REGIS") == 0  && ok == 1){
        //registrazione
        if(DEB) printf("Richiesta di Registrazione su socket %d\n", sClient);

        if(REGIST(buff,  client_tcp_addr) == NOTOK){
            simpleTCPmsg(sClient, GOBYE);
            close(sClient);
            connesso = 0;
        }
        else{
            simpleTCPmsg(sClient, WELCO);
            
            strncpy(id, buff+6, (ID_LEN-1));
            id[(ID_LEN-1)] = '\0';
            
            if(DEB) printf("User [ %s ] registrato e conneso su socket: %d\n", id ,sClient);
        }

        
        
    }


    else if(strcmp (type, "CONNE") == 0 && ok == 1){
        //connesione ad utente 
        if(DEB) printf("Richiesta di Connesione su socket %d\n", sClient);
        
        if(CONNECT(buff) == NOTOK){
            simpleTCPmsg(sClient, GOBYE);
            connesso = 0;
        }
        else{

            simpleTCPmsg(sClient, HELLO);
            
            strncpy(id, buff+6, (ID_LEN-1));
            id[(ID_LEN-1)] = '\0';
            
            if(DEB) printf("Connesione [ %s ] avvenuta con successo %d\n", id,sClient);
        }

        
    }   

    else{
        simpleTCPmsg(sClient, GOBYE);
        if(DEB) printf("Commando sconosciuto \"%s\" chiudo connesione\n", type);
        connesso = 0;

    }

    while (connesso){

        //if(DEB) printf("BUFF: %s ---\n", buff);


        if(readTCPmessage(sClient, buff, sizeof(buff)) == NOTOK){
            if(DEB) printf("il messaggio letto non è ok:  CHIUDO LA CONNESIONE su socket [ %d ]\n", sClient);
            simpleTCPmsg(sClient, GOBYE);
            break;
        }

       // if(DEB) printf("Thread [ socket: %d ] ha ricevuto:  %s\n", sClient, buff);

        strncpy(type, buff, 5);
        type[5]='\0';

        if(strcmp(type, "IQUIT") == 0){
            if(DEB) printf("Richiesta di chiusura della connesione\n");
            simpleTCPmsg(sClient, GOBYE);
            break;
        }



        else if(strcmp(type, "MESS?") == 0){
            if(DEB) printf("Richiesta di invio di messaggio\n");

            if(MESSAGE(buff, id) == NOTOK ){
                if(DEB) printf("Messaggio non inviato su socket [ %d ]\n", sClient);
                simpleTCPmsg(sClient, MESS_NOTOK);
            }
            else{
                if(DEB) printf("Messaggio inviato\n");
                simpleTCPmsg(sClient, MESS_OK);
            }

        }



        else if(strcmp(type, "FLOO?") == 0){
            if(DEB) printf("Richiesta di invio di FLOO\n");

        }





        else if(strcmp(type, "FRIE?") == 0){
            if(DEB) printf("Richiesta di amicizia\n");

            if(FREIREQ(buff, id) == NOTOK){
                if(DEB) printf("Richiesta non inviata su socket [ %d ]\n", sClient);
                simpleTCPmsg(sClient, FRIE_NOTOK);
            }
            else{
                if(DEB) printf("Richiaesta inviata\n");
                simpleTCPmsg(sClient, FRIE_OK);
            }
        }




        else if(strcmp(type, "LIST?") == 0){
            if(DEB) printf("Richiesta di lista utenti\n");

            if(LISTNUM(sClient) != NOTFIND){
                if(DEB) printf("Numero di client presenti inviato\n");

                if(LISTUSER(sClient) == NOTOK){
                    if(DEB) printf("Non sono stati inviati gli id degli utenti su socket [ %d ]\n", sClient);
                }

                if(DEB) printf("Invio di tutti i dati per richiesta lista utenti\n");

            }
            else{
                if(DEB) printf("Errore nel invio di numero di client presenti su socket [ %d ]\n", sClient);
            }
        }

        

        else if(strcmp(type, "CONSU") == 0){
            if(DEB) printf("Richiesta di consultazione di flussi\n");

        }




        else{
            simpleTCPmsg(sClient, GOBYE);
            if(DEB) printf("Commando sconosciuto \"%s\" chiudo connesione su socket [ %d ]\n", type, sClient);
            break;
        }

    }
    
   


    
    printf("=== Thread terminato per socket %d ===\n\n", sClient);
    close(sClient);
    pthread_exit(NULL);
}
