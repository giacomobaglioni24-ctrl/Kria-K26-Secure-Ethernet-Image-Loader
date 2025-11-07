#include <stdio.h>
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "lwip/tcp.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "tcp.h"
#include "server.h"
#include "network.h"



// Flag per i timer TCP
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

// Funzioni all'interno di network.c
void tcp_fasttmr(void);
void tcp_slowtmr(void);
void lwip_init();
void print_ip(char *msg, ip_addr_t *ip);
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);



// Stampa l'IPv4
void print_ip(char *msg, ip_addr_t *ip) {

	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));

}



// Stampa configurazioni IP
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw) {

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);

}



int NetworkConfig(struct netif *NetIf) {
    int Status = XST_SUCCESS;

    ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_ethernet_address[] = { 0x00U, 0x0AU, 0x35U, 0x15U, 0x0CU, 0x19U }; // Assegnato il MAC della KR260 


	// Addegna l'IP alla scheda e al Getaway e imposta la maschera
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);


	// Inizializza lo stack lwIP (Libreria che ci permette di usare comunicazione Ethernet)
	lwip_init();


	// Aggiunge l'interfaccia di rete, IP, Mask, IP gw, MAC, indirizzo fisico GEM
	if (!xemac_add(NetIf, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Errore nell'aggiunta dell'interfaccia di rete\n\r");
        Status = XST_FAILURE;
		goto END;
	}


	// La imposta come interfaccia di default
	netif_set_default(NetIf);

	// Dopo aver finito la configurazione abilita gli interrupt (DA CAPIRE SE VA BENE L'ORDINE)
	platform_enable_interrupts();

	// Attiva l'interfaccia
	netif_set_up(NetIf);

	// Stampa la configurazione IP
	print_ip_settings(&ipaddr, &netmask, &gw);


END:
    return Status;
}



void NetworkProcessPkt(struct netif *NetIf) {

    while (TRUE) {
		// TcpFastTmrFlag flag (variabile globale) settato da un interrupt timer hardware ogni 250ms
		if (TcpFastTmrFlag) {
			// Ogni volta che il flag si attiva viene chiamata tcp_fasttmr() che gestisce eventi TCP frequenti
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		// TcpSlowTmrFlag flag (variabile globale) settato da un interrupt timer hardware ogni 500ms
		if (TcpSlowTmrFlag) {
			// Ogni volta che il flag si attiva viene chiamata tcp_slowtmr() che gestisce eventi TCP meno frequenti
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		// xemacif_input(*) si occupa di gestire i pacchetti Ethernet della rete "*" e mandarli allo stack lwIP che si occupa dell'elaborazione
		xemacif_input(NetIf);
	}

}
