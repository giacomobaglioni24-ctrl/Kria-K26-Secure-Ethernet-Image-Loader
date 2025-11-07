#include <stdio.h>
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "tcp.h"
#include "network.h"
#include "server.h"
#include "xstatus.h"


// Stampa il nome della applicazione
void PrintAppHeader()
{
	xil_printf("\n\r\n\r-----lwIPv4 TCP server ------\n\r");
	xil_printf("TCP packets sent to port 1234 will be processed\n\r");
}



int main()
{
	int Status = XST_SUCCESS;

	struct netif NetIf = {0U};
	// Inizializza periferiche e hardware
	init_platform();

	Status = NetworkConfig(&NetIf);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nello start del Network\n\r");
        goto END;
    }


	Status = StartServer();
    if (Status == XST_SUCCESS) {
		PrintAppHeader();		
		NetworkProcessPkt(&NetIf);
    }
	else {
		xil_printf("Errore nello start del server TCP, errore %d\n\r", Status);
        goto END;
	}


END:
	xil_printf("\r\n\r\nApplicazione chiusa..................\r\n");
	cleanup_platform();
	return Status;
}



