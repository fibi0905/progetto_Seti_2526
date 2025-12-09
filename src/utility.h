#define OK 1
#define NOTOK 0
#define NOTFIND -1


//sarebbe 218 il massimo, visto che il messaggio più lungo 
//è l'invio di un messaggio dato che:
//MESS? -> 5byte + id-> 8byte + mess-> 200byte + "+++" ->3byte + "_ _" ->2byte  = per un totale di 218 
#define MAX_TCP_MESAGGE 256 
#define MAX_LEN 201
