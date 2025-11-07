#include <stdio.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "tcp.h"
#include "server.h"
#include "xil_printf.h"
#include "xstatus.h"



// Dichiarazioni di funzioni da mettere all'inizio del file
static err_t RecvCallback(void *arg, struct tcp_pcb *Tpcb, struct pbuf *p, err_t err);
static err_t AcceptCallback(void *arg, struct tcp_pcb *newpcb, err_t err);
TcpArgStruct* AllocTcpArg(void);



// Viene chiamata da lwIP ogni volta che riceve dati
static err_t RecvCallback(void *arg, struct tcp_pcb *Tpcb, struct pbuf *p, err_t err)
{
    TcpArgStruct *TcpArg = (TcpArgStruct *)arg;


	// Se il pacchetto � nullo (!p) vuol dire che la connessione � gi� stata chiusa dal client (PC)
    if ((err != ERR_OK) || (p == NULL)) {
        // Libera il packet buffer
		pbuf_free(p);
		// Chiude la connessione lato server (Scheda KR260)
		tcp_close(Tpcb);
		// Deregistra la callback al lwIP, il lwIP non chiamer� pi� callback a questo server
		tcp_recv(Tpcb, NULL);
		// Stampa sul terminale che la connessione � stata chiusa
		xil_printf("Connessione server chiusa\n\r");
		// Chiude la funzione e ritorna il flag "ERR_OK", che vuol dire che tutto � andato bene
        return ERR_OK;
    }


	// Assegno il PktCount e TotalBytes all struttura TcpArg relativa alla connessione per cui ho ricevuto la Callback
    TcpArg->PktCount++;
    TcpArg->TotalBytes += p->len;

    xil_printf("Connessione %u: ricevuto pacchetto %u (%u bytes totali)\n\r",
               TcpArg->ConnID, TcpArg->PktCount , TcpArg->TotalBytes);


	// Notifica lo stack lwIP che sono stati ricevuti p->len bytes di payload effettivi
    tcp_recved(Tpcb, p->len);
    

	// Qui gestisco il payload ricevuto, se pkt_count == 0 vuol dire che � il primo pacchetto ricevuto, altrimenti sono pacchetti successivi
	if(TcpArg->PktCount == 1) {
		xil_printf("Processando il primo pacchetto\n\r");
		ProcessReq(Tpcb, p->payload, p->len);
	}
	else {
		xil_printf("Processando il pacchetto nr. %d\n\r", TcpArg->PktCount);
		ProcessAdditionalPayload(Tpcb, p->payload, p->len);
	}


	// Libera il pbuf (packet buffer) cio� il buffer di ricezione quando ha ricevuto il pacchetto
    pbuf_free(p);


    return ERR_OK;
}



// Viene chiamata da lwIP ogni volta che un client (PC) si connette al server (Scheda KR260)
static err_t AcceptCallback(void *arg, struct tcp_pcb *newpcb, err_t err)
{	
    TcpArgStruct *TcpArg = AllocTcpArg();


    tcp_arg(newpcb, (void *)TcpArg);
	// Setta la callback "RecvCallback" che verr� chiamata ogni volta che il server riceve dati
    tcp_recv(newpcb, RecvCallback);

    xil_printf("Connessione %u stabilita\n\r", TcpArg->ConnID);

    return ERR_OK;
}



// Inizializza il server TCP Echo
int StartServer()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 1234;

	// Crea un nuovo PCB (Protocol Control Block) per gestire la connessione TCP, IPADDR_TYPE_ANY indica che il PCB pu� gestire sia connessioni IPv
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	// Assegna il PCB alla porta 7 , ogni server deve essere associato a una porta diversa, la porta viene scelta in base al servizio che si vuole offrire
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	// Impostiamo l'argomento che verr� passato a ogni callback della connessione a NULL, se volessimo integrare una chiave di autentizione dovremmo impostare
	// l'argomento a un puntatore alla chiave
	tcp_arg(pcb, NULL);

	// Mette il PCB in ascolto per le connessioni in entrata, se fallisce stampa un messaggio di errore
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	// Imposta la callback "AcceptCallback" che verr� chiamata ogni volta che un client (PC) si connette al server (Scheda KR260)
	tcp_accept(pcb, AcceptCallback);

	// Stampa sul terminale che il server echo � partito ed � associato alla porta 7
	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}
