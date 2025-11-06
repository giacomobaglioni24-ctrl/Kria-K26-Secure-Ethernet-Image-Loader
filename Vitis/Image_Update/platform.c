#include "xparameters.h"
#include "xparameters_ps.h"	/* defines XPAR values */
#include "xil_cache.h"
#include "xscugic.h"
#include "lwip/tcp.h"
#include "xil_printf.h"
#include "platform.h"
#include "platform_config.h"
#include "netif/xadapter.h"
#include "xttcps.h"
#include "dma.h"




static XTtcPs TimerInstance;
static XScuGic Intc;
static XInterval Interval;
static u8 Prescaler;

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

extern struct netif *NetIf;



void platform_clear_interrupt( XTtcPs * TimerInstance );



void TimerHandler(XTtcPs * TimerInstance)
{
	static int DetectEthLinkStatus = 0;
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified
	 * by lwIP. It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;

	DetectEthLinkStatus++;
    TcpFastTmrFlag = 1;
	odd = !odd;

	if (odd) {
		TcpSlowTmrFlag = 1;
	}

	/* For detecting Ethernet phy link status periodically */
	if (DetectEthLinkStatus == ETH_LINK_DETECT_INTERVAL) {
		eth_link_detect(NetIf);
		DetectEthLinkStatus = 0;
	}

	platform_clear_interrupt(TimerInstance);
}



void PlatformSetupTimer(void)
{
	int Status;
	XTtcPs * Timer = &TimerInstance;
	XTtcPs_Config *Config;


	Config = XTtcPs_LookupConfig(TIMER_DEVICE_ID);

	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("In %s: Timer Cfg initialization failed...\r\n",
				__func__);
				return;
	}
	XTtcPs_SetOptions(Timer, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
	XTtcPs_CalcIntervalFromFreq(Timer, PLATFORM_TIMER_INTR_RATE_HZ, &Interval, &Prescaler);
	XTtcPs_SetInterval(Timer, Interval);
	XTtcPs_SetPrescaler(Timer, Prescaler);
}



void platform_clear_interrupt( XTtcPs * TimerInstance )
{
	u32 StatusEvent;

	StatusEvent = XTtcPs_GetInterruptStatus(TimerInstance);
	XTtcPs_ClearInterruptStatus(TimerInstance, StatusEvent);
}




void PlatformEnableInterrupts()
{

	// Abilita gli interrupt
    XScuGic_Enable(&Intc, TX_INTR_ID);
    XScuGic_Enable(&Intc, RX_INTR_ID);
    XScuGic_Enable(&Intc, TIMER_IRPT_INTR);


    // Abilita gli interrupt del timer (driver)
    XTtcPs_EnableInterrupts(&TimerInstance, XTTCPS_IXR_INTERVAL_MASK);
    XTtcPs_Start(&TimerInstance);


    // Inizializza e abilita le eccezioni globali
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);
    Xil_ExceptionEnable();

	xil_printf("All interrupts enabled\r\n");

	return;
}



int PlatformSetupInterrupts(XAxiDma *AxiDmaPtr){
	int Status;
	
	XScuGic_Config *IntcConfig;


	// Inizializza il controller di interrupt
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		goto END;
	}

	Status = XScuGic_CfgInitialize(&Intc, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	
	// Configura priorità e tipo di trigger per gli interrupt
	XScuGic_SetPriorityTriggerType(&Intc, TX_INTR_ID, 0xA0, 0x3);
	XScuGic_SetPriorityTriggerType(&Intc, RX_INTR_ID, 0xA0, 0x3);
	//XScuGic_SetPriorityTriggerType(&Intc, TIMER_IRPT_INTR, 0xA0, 0x1);


    // Collega gli handler DMA e l'interrupt del timer 
    Status = XScuGic_Connect(&Intc, TX_INTR_ID, (Xil_InterruptHandler)TxIntrHandler, AxiDmaPtr);
    if (Status != XST_SUCCESS) {
		goto END;
	}

    Status = XScuGic_Connect(&Intc, RX_INTR_ID, (Xil_InterruptHandler)RxIntrHandler, AxiDmaPtr);
    if (Status != XST_SUCCESS) {
		goto END;
	}

    Status = XScuGic_Connect(&Intc, TIMER_IRPT_INTR, (Xil_InterruptHandler)TimerHandler, &TimerInstance);
    if (Status != XST_SUCCESS) {
		goto END;
	}


END:
	return Status;
}



void PlatformInit()
{
	PlatformSetupTimer();

	return;
}



void CleanupPlatform()
{
	Xil_ICacheDisable();
	Xil_DCacheDisable();
	return;
}

