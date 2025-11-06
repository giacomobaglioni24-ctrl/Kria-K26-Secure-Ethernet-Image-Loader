#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xdebug.h"
#include "xil_util.h"
#include "xinterrupt_wrap.h"
#include "xscugic.h"
#include "dma.h"
#include "platform.h"



XAxiDma AxiDma;		
volatile u32 TxDone;
volatile u32 RxDone;
volatile u32 Error;



int DMA_Init()
{
	int Status = XST_SUCCESS;
	XAxiDma_Config *Config;


	Config = XAxiDma_LookupConfig(DMA_DEV_ID);
	if (!Config) {
		xil_printf("No config found for %d\r\n", DMA_DEV_ID);
        goto END;
	}
	//xil_printf("DMA Configuration found\r\n");

	Status = XAxiDma_CfgInitialize(&AxiDma, Config);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed %d\r\n", Status);
		goto END;
	}
	//xil_printf("DMA Initialization done\r\n");

	if (XAxiDma_HasSg(&AxiDma)) {
		xil_printf("Device configured as SG mode \r\n");
		goto END;
	}
	//xil_printf("Device configured as Simple mode \r\n");

	
	// Setup the interrupt system
	Status = PlatformSetupInterrupts(&AxiDma);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed intr setup\r\n");
		goto END;
	}


	// Disabilita tutti gli interrupt prima di abilitarli
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	//xil_printf("Interrupts disabled\r\n");

	// Abilita tutti gli interrupt
	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	//xil_printf("Interrupts enabled\r\n");


END:    
	return Status;
}



int DMA_Transfer(u32 Length) {
    int Status = XST_SUCCESS;
	u8 *TxBufferPtr;
	u8 *RxBufferPtr;
    u32 Offset = 0;


	TxBufferPtr = (u8 *)TX_BUFFER_BASE;
	RxBufferPtr = (u8 *)RX_BUFFER_BASE;


	// Flush the SrcBuffer before the DMA transfer, in case the Data Cache is enabled
	Xil_DCacheFlushRange((UINTPTR) TxBufferPtr, Length);
	Xil_DCacheFlushRange((UINTPTR) RxBufferPtr, Length);


	TxDone = 0;
	RxDone = 0;
	Error = 0;


	Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR) RxBufferPtr, Length, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR) TxBufferPtr, Length, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	
	Status = Xil_WaitForEventSet(POLL_TIMEOUT_COUNTER, NUMBER_OF_EVENTS, &Error);
	if (Status == XST_SUCCESS) {
		if (!TxDone) {
			xil_printf("Transmit error %d\r\n", Status);
			goto END;
		} else if (Status == XST_SUCCESS && !RxDone) {
			xil_printf("Receive error %d\r\n", Status);
			goto END;
		}
	}
		
	Status = Xil_WaitForEventSet(POLL_TIMEOUT_COUNTER, NUMBER_OF_EVENTS, &TxDone);
	if (Status != XST_SUCCESS) {
		xil_printf("Transmit failed %d\r\n", Status);
		goto END;
	}

	Status = Xil_WaitForEventSet(POLL_TIMEOUT_COUNTER, NUMBER_OF_EVENTS, &RxDone);
	if (Status != XST_SUCCESS) {
		xil_printf("Receive failed %d\r\n", Status);
		goto END;
	}


END:
	return Status;
}



//  Callback per l'interrupt di trasmissione DMA
void TxIntrHandler(void *Callback)
{
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *AxiDmaInst = (XAxiDma *)Callback;

	//xil_printf("TX Interrupt Handler executed\r\n");

	// Legge gli interrupt pendenti
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DMA_TO_DEVICE);

	// Fa l'acknowledge degli interrupt pendenti
	XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DMA_TO_DEVICE);


	// Se non c'è nessun interrupt attivo, non fare nulla
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {

		goto END;
	}


	// Se c'è un errore, setta il flag di errore e resetta il DMA
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		Error = 1;

	
		// Resetta il DMA
		XAxiDma_Reset(AxiDmaInst);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(AxiDmaInst)) {
				break;
			}

			TimeOut -= 1;
		}

		goto END;
	}

	
	// Se c'è un interrupt di completamento, setta il flag TxDone
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {

		TxDone = 1;
	}


END:
	return;
}



void RxIntrHandler(void *Callback)
{
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *AxiDmaInst = (XAxiDma *)Callback;

	//xil_printf("RX Interrupt Handler executed\r\n");

	// Legge gli interrupt pendenti
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DEVICE_TO_DMA);

	// Fa l'acknowledge degli interrupt pendenti
	XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

	
	// Se non c'è nessun interrupt attivo, non fare nulla
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		goto END;
	}

	// Se c'è un errore, setta il flag di errore e resetta il DMA
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		Error = 1;

		// Resetta il DMA
		XAxiDma_Reset(AxiDmaInst);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(AxiDmaInst)) {
				break;
			}

			TimeOut -= 1;
		}

		goto END;
	}


	// Se c'è un interrupt di completamento, setta il flag RxDone
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {

		RxDone = 1;
	}


END:
	return;
}
