#include "Server.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


static user listUser [MAX_CLIENT];
static unsigned int nUser = 0;

pthread_mutex_t semList = PTHREAD_MUTEX_INITIALIZER;

//fare una fuzione per inizializare il serve (vedi gemini)
