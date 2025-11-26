#ifndef PLATFORM_H
#define PLATFORM_H

#include "xaxidma.h"


#define ETH_LINK_DETECT_INTERVAL 4

// Per gli interrupt 
#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID		XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_DEVICE_ID		XPAR_XTTCPS_0_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_XTTCPS_0_INTR
#define INTC_BASE_ADDR		XPAR_SCUGIC_0_CPU_BASEADDR
#define INTC_DIST_BASE_ADDR	XPAR_SCUGIC_0_DIST_BASEADDR
#define PLATFORM_TIMER_INTR_RATE_HZ (4)


void PlatformInit();
void CleanupPlatform();
void PlatformSetupTimer();
void PlatformEnableInterrupts();
int PlatformSetupInterrupts(XAxiDma *AxiDmaPtr);

#endif // PLATFORM_H

