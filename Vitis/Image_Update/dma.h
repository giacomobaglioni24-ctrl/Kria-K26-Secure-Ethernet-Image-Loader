#ifndef DMA_H
#define DMA_H

#include "xil_types.h"


#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#define DDR_BASE_ADDR	XPAR_PSU_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x01100000)

#define RESET_TIMEOUT_COUNTER	(10000U)
#define MAX_PKT_LEN (0x10000U) // 64KB
#define POLL_TIMEOUT_COUNTER (50000000U) // ~500ms
#define NUMBER_OF_EVENTS	(1U)


int DMA_Init();
int DMA_Transfer(u32 Length);
void RxIntrHandler(void *Callback);
void TxIntrHandler(void *Callback);

#endif // DMA_H
