#include <stdio.h>
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "tcp.h"
#include "network.h"
#include "server.h"
#include "xstatus.h"
#include "qspi.h"
#include "dma.h"


// Stampa il nome della applicazione
void PrintAppHeader()
{
	xil_printf("\n\r\n\r------ Image Update via Ethernet DDR DMA started ------\n\r");
}



int main()
{
	int Status = XST_SUCCESS;

	struct netif NetIf = {0U};

	
	PlatformInit();


	Status = QSPI_Init(); 
    if ( Status != XST_SUCCESS ) {
        xil_printf("Errore nello start della QSPI, errore: %d\r\n", Status);
    }


	Status = DMA_Init(); 
    if ( Status != XST_SUCCESS ) {
        xil_printf("Errore nello start del DMA, errore: %d\r\n", Status);
    }


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
	CleanupPlatform();
	return Status;
}



