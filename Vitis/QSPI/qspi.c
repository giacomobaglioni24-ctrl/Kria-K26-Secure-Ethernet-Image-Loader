#include <stdio.h>
#include "xqspipsu.h"
#include "platform.h"
#include "xil_printf.h"
#include "qspi.h"
#include "err.h"
#include "sys.h"



// Inizializzo il puntatore alla struttura dell'istanza dei driver QSPI
XQspiPsu QspiInstance;
//static u8 DataBuffer[200000U];

// Definizione variabili globali
static u8 RdCmd = QUAD_READ_CMD_4BYTE;  // Comando di lettura
static u8 WrCmd = QUAD_INPUT_FAST_PROGRAM_4BYTE;
static u8 WrEnCmd = WRITE_ENABLE_CMD;
static u8 SecErCmd = SECTOR_ERASE_4BYTE;
static u8 FlagRegister = {0U};

// Funzioni all'interno di qspi.c
static int Enable4ByteAddrMode();
static int ReadRegisters(u8 *Flag_Val_Ptr, ClearStatusFlag Enable);



static int Enable4ByteAddrMode() {
    int Status = XST_FAILURE;

    u8 Cmd = ENTER_4B_ADDR_MODE;
    XQspiPsu_Msg FlashMsg[2U] = {0U};

    // Comando di entrata in 4 byte address mode
    FlashMsg[0U].TxBfrPtr = &Cmd;
	FlashMsg[0U].RxBfrPtr = NULL;
	FlashMsg[0U].ByteCount =1U;
	FlashMsg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(&QspiInstance, FlashMsg, 1U);
	if (Status != XST_SUCCESS) {
		goto END;
        Status = XST_ERROR_QSPI_4BYTE_ENTER;
	}
    //xil_printf("Enter 4 byte address mode command succesfully sent\r\n");

    END:
    	return Status;
}



int XQspi_Init () {
    int Status = XST_FAILURE;

    XQspiPsu_Config *QspiConfig;

    // Guarda la configurazione del dispositivo QSPI basata sull'ID del dispositivo, ritorna un puntatore alla struttura di configurazione
    QspiConfig = XQspiPsu_LookupConfig(XPAR_XQSPIPSU_0_DEVICE_ID); 
    // Se il puntatore è nullo vuol dire che non è stata trovata la configurazione
    if (QspiConfig == NULL) {
        xil_printf("\r\nNo config found for %d\r\n", XPAR_XQSPIPSU_0_DEVICE_ID);
        Status = XST_ERROR_QSPI_CONFIG;
        goto END;
    }
    //xil_printf("\r\nConfig found\r\n");

    // Inizializza l'istanza del driver QSPI con la configurazione specificata, EffectiveAddr è l'indirizzo base del dispositivo, ritorna uno status che indica se l'inizializzazione è andata a buon fine
    Status = XQspiPsu_CfgInitialize(&QspiInstance, QspiConfig, QspiConfig->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("Initialization failed error n°%d\r\n", Status);
        Status = XST_ERROR_QSPI_CONFIG_INIT;
        goto END;
    }
    //xil_printf("Initialization done\r\n");

    // Impostiamo il controller QSPI in modalità manual start
    Status = XQspiPsu_SetOptions(&QspiInstance, XQSPIPSU_MANUAL_START_OPTION); 
    if (Status != XST_SUCCESS) {
        xil_printf("Set options failed error n°%d\r\n", Status);
        Status = XST_ERROR_QSPI_MANUAL_START;
        goto END;
    }
    //xil_printf("Set options done\r\n");

    // Impostiamo il clock prescaler (dipende dalla frequenza del clock di ingresso e dalla frequenza massima supportata dalla flash)
    Status = XQspiPsu_SetClkPrescaler(&QspiInstance, XQSPIPSU_CLK_PRESCALE_8);
	if (Status != XST_SUCCESS) {
		Status = XST_ERROR_QSPI_PRESCALER_CLK;
		goto END;
	}
    //xil_printf("Set prescaler done\r\n");

    // Selezioniamo la flash, visto che ho impostato single connection mode nel .xsa, metto XQSPIPSU_SELECT_FLASH_BUS_LOWER e XQSPIPSU_SELECT_FLASH_CS_LOWER
	XQspiPsu_SelectFlash(&QspiInstance,
			XQSPIPSU_SELECT_FLASH_CS_LOWER,
			XQSPIPSU_SELECT_FLASH_BUS_LOWER);
    //xil_printf("Flash selected\r\n");

    // Abilito la 4 byte address mode
    Status = Enable4ByteAddrMode();

    END:
    	return Status;
}



int ReadFlash(u32 SrcAddress, u8 *ReadBuffer, u32 ByteCount) {
    int Status = XST_SUCCESS;

    u8 tx_buf[5U] = {0U};
    XQspiPsu_Msg Msg[3U] = {0U};


    tx_buf[COMMAND_OFFSET] = RdCmd;
    tx_buf[ADDRESS_1_OFFSET] = (u8)((SrcAddress & 0xFF000000U) >> 24U);
    tx_buf[ADDRESS_2_OFFSET] = (u8)((SrcAddress & 0xFF0000U) >> 16U);
    tx_buf[ADDRESS_3_OFFSET] = (u8)((SrcAddress & 0xFF00U) >> 8U);
    tx_buf[ADDRESS_4_OFFSET] = (u8)(SrcAddress & 0xFFU);

    xil_printf("\r\nLettura su indirizzo 0x%02X%02X%02X%02X\r\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4]);

    Msg[0U].TxBfrPtr = tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;		
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = NULL;
	Msg[1U].RxBfrPtr = NULL;
	Msg[1U].ByteCount = DUMMY_CLOCKS;
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
	Msg[1U].Flags = 0;

    Msg[2U].TxBfrPtr = NULL;
    Msg[2U].RxBfrPtr = ReadBuffer;//rx_buf;
    Msg[2U].ByteCount = ByteCount;//rx_buf);
    Msg[2U].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
    Msg[2U].Flags = XQSPIPSU_MSG_FLAG_RX;

    // Esegui il trasferimento in polling
    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 3);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella lettura dalla flash.\n");
        Status = XST_ERROR_QSPI_READ;
        goto END;
    }

    
    END:
        return Status;
}



int WriteFlash(u32 DstAddress, u8 *WriteBuffer, u32 ByteCount) {
    int Status = XST_SUCCESS;

    u8 tx_buf[5U] = {0U};
    XQspiPsu_Msg Msg[2U] = {0U};


    // Mando il WE prima di scrivere 

    Msg[0U].TxBfrPtr = &WrEnCmd;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = 1;
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI; // Comando e indirizzo in SPI
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 1);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nell'invio del WE.\r\n");
        goto END;
    }

    //xil_printf("\r\nWrite Enable abilitato:\r\n");

    //ReadRegisters(&FlagRegister, CLEAR_DISABLE);


    tx_buf[COMMAND_OFFSET] = WrCmd;
    tx_buf[ADDRESS_1_OFFSET] = (u8)((DstAddress & 0xFF000000U) >> 24U);
    tx_buf[ADDRESS_2_OFFSET] = (u8)((DstAddress & 0xFF0000U) >> 16U);
    tx_buf[ADDRESS_3_OFFSET] = (u8)((DstAddress & 0xFF00U) >> 8U);
    tx_buf[ADDRESS_4_OFFSET] = (u8)(DstAddress & 0xFFU);

    xil_printf("\r\nScrittura su indirizzo 0x%02X%02X%02X%02X\r\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4]);

    Msg[0U].TxBfrPtr = tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI; 
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = WriteBuffer;//data_buf;
    Msg[1U].RxBfrPtr = NULL;
    Msg[1U].ByteCount = ByteCount;//data_buf);
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI; 
    Msg[1U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella scrittura sulla flash.\r\n");
        goto END;
    }

    while (1) {

        ReadRegisters(&FlagRegister, CLEAR_DISABLE);

        if ( FlagRegister == 0x81 ) {
        	xil_printf("-> Scrittura completata\r\n");
        	goto END;
        }
        else {
            xil_printf("-> Scrittura in corso\n\r");
        }

        usleep(1);

    }


END:
    return Status;
}



int SectorErase(u32 Address) {
    int Status = XST_SUCCESS;

    u8 tx_buf[5U] = {0U};
    XQspiPsu_Msg Msg[1U] = {0U};


    // Abilito il WE prima di fare l'erase

    Msg[0U].TxBfrPtr = &WrEnCmd;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = 1;
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI; // Comando e indirizzo in SPI
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 1);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nell'invio del WE.\r\n");
        goto END;
    }

    //xil_printf("\r\nWrite Enable abilitato:\r\n");

    //ReadRegisters(&FlagRegister, CLEAR_DISABLE);


    tx_buf[COMMAND_OFFSET] = SecErCmd;
    tx_buf[ADDRESS_1_OFFSET] = (u8)((Address & 0xFF000000U) >> 24U);
    tx_buf[ADDRESS_2_OFFSET] = (u8)((Address & 0xFF0000U) >> 16U);
    tx_buf[ADDRESS_3_OFFSET] = (u8)((Address & 0xFF00U) >> 8U);
    tx_buf[ADDRESS_4_OFFSET] = (u8)(Address & 0xFFU);

    xil_printf("\r\nSector Erase su ADDR: 0x%02X%02X%02X%02X\r\n", tx_buf[ADDRESS_1_OFFSET], tx_buf[ADDRESS_2_OFFSET], tx_buf[ADDRESS_3_OFFSET], tx_buf[ADDRESS_4_OFFSET]);

    Msg[0U].TxBfrPtr = tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI; // Comando e indirizzo in SPI
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 1);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nell'erase del settore.\r\n");
        Status = XST_ERROR_SECTOR_ERASE;
        goto END;
    }

    while (1) {

        ReadRegisters(&FlagRegister, CLEAR_DISABLE);

        if ( FlagRegister == 0x81 ) {
        	xil_printf("-> Il settore � stato cancellato\n\r");
        	goto END;
        }
        else {
            xil_printf("-> Il settore sta venendo cancellato\n\r");
        }

        usleep(1);

    }


END:
    return Status;
}



static int ReadRegisters(u8 *Flag_Val_Ptr, ClearStatusFlag Enable) {

    int Status = XST_SUCCESS;
    XQspiPsu_Msg Msg[2U] = {0U};
    u8 rx_buf = {0U};

    // Leggo il Flag Status Register
    u8 tx_buf = READ_FLAG_STATUS_CMD;

    Msg[0U].TxBfrPtr = &tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = NULL;
    Msg[1U].RxBfrPtr = Flag_Val_Ptr;
    Msg[1U].ByteCount = sizeof(*Flag_Val_Ptr);
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[1U].Flags = XQSPIPSU_MSG_FLAG_RX;

    // Esegui il trasferimento in polling
    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella lettura del Flag Status Register.\n");
        goto END;
    }

    // Stampa i dati letti
    //xil_printf("\r\nFlag Status Register: 0x%02X\r\n", *Flag_Val_Ptr);


    // Se è attivo l'Enable facciamo il Clear dello Status Flag Register
    if ( Enable == CLEAR_ENABLE ) {
        tx_buf = CLEAR_FLAG_STATUS_CMD;

        Msg[0].TxBfrPtr = &tx_buf;
        Msg[0].RxBfrPtr = NULL;
        Msg[0].ByteCount = sizeof(tx_buf);
        Msg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
        Msg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

        Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
        if (Status != XST_SUCCESS) {
            xil_printf("Errore nel clear dei registri di status flag.\n");
            goto END;
        }
    }
    

    // Leggo lo Status Register
    tx_buf = READ_STATUS_CMD;

    Msg[0U].TxBfrPtr = &tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = NULL;
    Msg[1U].RxBfrPtr = &rx_buf;
    Msg[1U].ByteCount = sizeof(rx_buf);
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[1U].Flags = XQSPIPSU_MSG_FLAG_RX;

    // Esegui il trasferimento in polling
    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella lettura dello Status Register.\r\n");
        goto END;
    }

    // Stampa i dati letti
    //xil_printf("Status Register: 0x%02X\r\n", rx_buf);


    // Leggo il Enhanced Volatile Configuration Register
    tx_buf = READ_ENHANCED_VOLATILE_CONFIGURATION_CMD;

    Msg[0U].TxBfrPtr = &tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = NULL;
    Msg[1U].RxBfrPtr = &rx_buf;
    Msg[1U].ByteCount = sizeof(rx_buf);
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[1U].Flags = XQSPIPSU_MSG_FLAG_RX;

    // Esegui il trasferimento in polling
    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella lettura dell' Enhanced Volatile Configuration Register.\r\n");
        goto END;
    }

    // Stampa i dati letti
    //xil_printf("Enhanced Volatile Configuration Register: 0x%02X\r\n", rx_buf);


    // Leggo il Volatile Configuration Register
    tx_buf = READ_VOLATILE_CONFIGURATION_CMD;

    Msg[0U].TxBfrPtr = &tx_buf;
    Msg[0U].RxBfrPtr = NULL;
    Msg[0U].ByteCount = sizeof(tx_buf);
    Msg[0U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[0U].Flags = XQSPIPSU_MSG_FLAG_TX;

    Msg[1U].TxBfrPtr = NULL;
    Msg[1U].RxBfrPtr = &rx_buf;
    Msg[1U].ByteCount = sizeof(rx_buf);
    Msg[1U].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    Msg[1U].Flags = XQSPIPSU_MSG_FLAG_RX;

    // Esegui il trasferimento in polling
    Status = XQspiPsu_PolledTransfer(&QspiInstance, Msg, 2);
    if (Status != XST_SUCCESS) {
        xil_printf("Errore nella lettura del Volatile Configuration Register.\r\n");
        goto END;
    }

    // Stampa i dati letti
    //xil_printf("Volatile Configuration Register: 0x%02X\r\n", rx_buf);


    END:
        return Status;
}
