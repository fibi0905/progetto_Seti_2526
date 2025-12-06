#include "Server.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


static user listUser [MAX_CLIENT];
static unsigned int nUser = 0;

pthread_mutex_t semList = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semNuser = PTHREAD_MUTEX_INITIALIZER;

//fare una fuzione per inizializare il serve (vedi gemini)


unsigned int serverInit (){
    printf ("server Start: ");

    if(pthread_mutex_init(&semList, NULL) != 0 && pthread_mutex_init(&semNuser, NULL) != 0 ){
        perror("server init");
        return NOTOK;
    }

    return OK;
}


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


unsigned int addUser(const char id [ID_LEN], struct sockaddr_in add, unsigned int pass ){
    int pox;
    if((pox = findUser(id)) != NOTFIND) return modUser(pox, add); //potrebbe non aver senso 
    
    user newUser;

    strcpy(newUser.id, id);
    newUser.add = add;
    newUser.pass = pass;
    newUser.nMsg = 0;
    newUser.nMsg = NULL;
    newUser.nFri = 0;

    pthread_mutex_lock(&semNuser);
    pthread_mutex_lock (&semList); 

    nUser ++;
    listUser[nUser] = newUser;

    pthread_mutex_unlock(&semNuser);
    pthread_mutex_unlock(&semList);

    return OK;
}


