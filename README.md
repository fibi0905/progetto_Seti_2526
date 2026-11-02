# Progetto SETI 2025/2026 – IPortbook

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![TCP/IP](https://img.shields.io/badge/TCP%2FUDP-Networking-0A66C2?style=for-the-badge)
![Client-Server](https://img.shields.io/badge/Architecture-Client--Server-FF6F00?style=for-the-badge)
![University Project](https://img.shields.io/badge/University-Project-8E24AA?style=for-the-badge)
![Teamwork](https://img.shields.io/badge/Collaboration-Teamwork-43A047?style=for-the-badge)
![Specification](https://img.shields.io/badge/Development-Specification_Driven-546E7A?style=for-the-badge)


![Server by Fabio Nicolosi](https://img.shields.io/badge/Server-Fabio_Nicolosi-blue?style=for-the-badge)
![Client by Davide Ferraro](https://img.shields.io/badge/Client-Davide_Ferraro-green?style=for-the-badge)

<br></br>

---
## Presentazione

Questo repository contiene il progetto sviluppato per il corso universitario **SETI 2025/2026 (Sistemi di Elaborazione e Trasmissione dell'Informazione)**.

L'obiettivo del progetto era realizzare un sistema **Client-Server** per la gestione di una rubrica di rete, implementando la comunicazione tramite **TCP** e **UDP** e rispettando rigorosamente le specifiche tecniche fornite dal docente.

Il progetto è stato sviluppato interamente seguendo il documento di specifica (`progetto-seti25.pdf`), simulando un contesto di sviluppo reale in cui i requisiti vengono definiti da un committente e devono essere implementati senza modificarne il comportamento.

<br></br>


---

# Architettura del progetto

Il progetto è stato sviluppato seguendo un'architettura **Client-Server**, nella quale il client comunica con il server utilizzando i protocolli **TCP** e **UDP** in base alle operazioni previste dalle specifiche del progetto.

```text
                         Comunicazione TCP / UDP

    +------------------------------------------------------+
    |                      CLIENT                          |
    |------------------------------------------------------|
    | • Interfaccia a riga di comando                      |
    | • Gestione input utente                              |
    | • Invio richieste al server                          |
    | • Ricezione delle risposte                           |
    |                                                      |
    +-------------------------+----------------------------+
                              │
                              │
                              │
                    TCP / UDP Socket API
                              │
                              ▼
    +------------------------------------------------------+
    |                      SERVER                          |
    |------------------------------------------------------|
    | • Gestione connessioni                               |
    | • Parsing delle richieste                            |
    | • Logica applicativa                                 |
    | • Gestione dei protocolli TCP e UDP                  |
    | • Elaborazione delle risposte                        |
    |                                                      |
    +------------------------------------------------------+
```


<br></br>

---

# Competenze dimostrate

Questo progetto mette in evidenza principalmente:

- 🤝 **Capacità di lavorare in team**, attraverso una chiara suddivisione delle responsabilità tra client e server.
- 📋 **Capacità di seguire specifiche tecniche**, implementando fedelmente quanto richiesto dal documento di progetto.
- 🌐 **Conoscenza delle comunicazioni di rete**, con particolare attenzione ai protocolli **TCP** e **UDP**.
- ⚙️ Progettazione di un'architettura Client-Server.
- 💻 Sviluppo in linguaggio C.
- 🐞 Debug e gestione degli errori.

<br></br>

---

# Suddivisione del lavoro


Il progetto è stato sviluppato in collaborazione tra due persone con una netta divisione delle responsabilità.

| Sviluppatore | Responsabilità |
|--------------|----------------|
| **Fabio Nicolosi** | **Progettazione e sviluppo completo del SERVER** |
| **Davide Ferraro** | **Progettazione e sviluppo completo del CLIENT** |

## [Fabio Nicolosi](https://www.linkedin.com/in/fabio-nicolosi-ge/)

Mi sono occupato della progettazione e dello sviluppo dell'intera parte **Server**, comprendendo:

- progettazione dell'architettura del server;
- implementazione della logica applicativa;
- gestione della comunicazione TCP;
- gestione della comunicazione UDP;
- progettazione del protocollo di comunicazione;
- gestione delle richieste e delle risposte;
- debug e test del server.

## [Davide Ferraro](https://www.linkedin.com/in/davideferraroIT)

Si è occupato della progettazione e dello sviluppo dell'intera parte **Client**, comprendendo:

- implementazione del client;
- gestione della comunicazione con il server;
- logica lato client;
- gestione dell'interfaccia da riga di comando prevista dal progetto.

<br></br>


> **Nota:** la suddivisione del lavoro è stata completamente separata. **Fabio Nicolosi ha progettato e sviluppato il SERVER**, mentre **Davide Ferraro ha progettato e sviluppato il CLIENT**.

<br></br>

---

# Specifiche del progetto

L'intero progetto è stato sviluppato seguendo le specifiche contenute nel file:

```
progetto-seti25.pdf
```

Il documento definisce nel dettaglio:

- architettura del sistema;
- formato dei messaggi;
- comportamento del client;
- comportamento del server;
- protocolli di comunicazione;
- requisiti funzionali richiesti.

<br></br>


---

# Struttura del repository

```text
progetto_Seti_2526/
├── Client/
│   ├── Client.c
│   ├── Client.h
│   └── libClient.c
│
├── Server/
│   ├── Server.c
│   ├── Server.h
│   └── libServer.c
│
├── src/
│   ├── utility.c
│   └── utility.h
│
├── progetto-seti25.pdf      # Specifiche ufficiali del progetto
├── README.md
└── .gitignore
```


<br></br>


---

# Compilazione

## Server

```bash
gcc -Wall ./libServer.c ../src/utility.c ./Server.c -o server
```

### Modalità Debug

Avviando il server con:

```bash
./server -d
```

vengono abilitate le stampe di debug sul terminale.

### Parametri

| Parametro | Valore |
|-----------|--------|
| IP | Indirizzo della macchina host (es. `127.0.0.1`) |
| Porta | `8080` |

---

## Client

```bash
gcc -Wall Client.c libClient.c ../src/utility.c -o client
```

### Opzioni disponibili

| Opzione | Descrizione |
|---------|-------------|
| `-c` | Utilizza le credenziali predefinite presenti nel codice (solo per test). |
| `-d` | Abilita la modalità debug (verbose). |

---

# Obiettivi formativi

Questo progetto ha permesso di consolidare competenze riguardanti:

- sviluppo di applicazioni Client-Server;
- utilizzo dei socket BSD;
- comunicazione tramite TCP e UDP;
- progettazione di protocolli applicativi;
- gestione della concorrenza;
- organizzazione del codice in moduli;
- collaborazione in team seguendo una specifica tecnica.

<br></br>


---

# Licenza

Questa opera è sotto 
[Licenza Creative Commons Attribuzione 4.0 Internazionale][cc-by].

[![CC BY 4.0][cc-by-image]][cc-by]

[cc-by]: http://creativecommons.org/licenses/by/4.0/deed.it
[cc-by-image]: https://i.creativecommons.org/l/by/4.0/88x31.png
[cc-by-shield]: https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg
