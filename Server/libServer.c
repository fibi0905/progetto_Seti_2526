#include "Server.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*-------Variabili & Semafori-------*/


//-------Gestione User
static user listUser [MAX_CLIENT];
static unsigned int nUser = 0;
pthread_mutex_t semList = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semNuser = PTHREAD_MUTEX_INITIALIZER;


//-------Gestione UDP
int udpSocket = -1;
pthread_mutex_t semUDP = PTHREAD_MUTEX_INITIALIZER;


/*-------------------Funzioni Gestione Utenti-------------------*/

int findUser(const char id [ID_LEN]){
    pthread_mutex_lock(&semNuser);
    if(nUser == 0) return NOTFIND;
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

unsigned int modUser (const unsigned int pox, struct sockaddr_in add);

unsigned int addUser(const char id [ID_LEN], struct sockaddr_in add, unsigned int pass ){
    int pox;
    if((pox = findUser(id)) != NOTFIND) return modUser(pox, add); //potrebbe non aver senso 
    
 

    pthread_mutex_lock(&semNuser);
    //IL SERVER è PIENO 
    if(nUser >= MAX_CLIENT){
        pthread_mutex_unlock(&semNuser);
        return NOTOK;
    }


    pthread_mutex_lock (&semList); 

    user newUser;
    strcpy(newUser.id, id);
    newUser.add = add;
    newUser.pass = pass;
    newUser.nMsg = 0;
    newUser.listMsg = NULL;
    newUser.nFri = 0;

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
    if( findUser (idS) == NOTFIND || (pox =findUser(idR)) == NOTFIND ) return NOTOK;
    

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


    return OK;
}



unsigned int addMSG (const char idD, const char idS[ID_LEN], const char value[MAX_LEN], typFlux tip);


/*-------------------Funzioni TCP e UDP-------------------*/

unsigned int serverInit (){
    printf ("server Start: ");

    if(pthread_mutex_init(&semList, NULL) != 0 || 
        pthread_mutex_init(&semNuser, NULL) != 0  || 
            pthread_mutex_init(&semUDP, NULL) != 0 
    ){

        perror("server init");
        return NOTOK;
    }

    //Creazione socket UDP
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) {
        perror("UDP socket creation");
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
}


unsigned int simpleUDPmsg (int sock, typSimpleMsg tip){
    const char msg [9];
    switch (tip){
        case WELCO: strcmp(msg, "WELCO+++\0");
        case GOBYE: strcmp(msg, "GOBYE+++\0");
        case FRIE_OK: strcmp(msg, "FRIE>+++\0");
        case FRIE_NOTOK: strcmp(msg, "FRIE<+++\0");
        case MESS_OK: strcmp(msg, "MESS>+++\0");
        case MESS_NOTOK: strcmp(msg, "MESS<+++\0");
        case FLOO_OK: strcmp(msg, "FLOO>+++\0");
        case ACKRF: strcmp(msg, "ACKRF+++\0");
        case NOCON: strcmp(msg, "NOCON+++\0");
          
        default:
            return NOTOK;
    }

    ssize_t byteSent = send(sock, msg, strlen(msg), 0);  
    if(byteSent <= 0) return NOTOK;

    return OK;

}

