#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sys.h"
#include "xstatus.h"
#include "qspi.h"
#include "tcp.h"
#include "err.h"
#include "dma.h"



static u32 CurrentAddress = {0U};
static u32 RemainingImgData = {0U};
static u32 ImageSize = {0U};



static SysBootImgInfo BootImgInfo;
static SysUploadImgInfo UploadImgInfo;



static int ImageWrite(u32 DstAddress, u8 *WriteBuffer, u32 ByteCount) {
    int Status = XST_FAILURE;

    u32 DataSize = ByteCount;
	u32 WrAddr = DstAddress;
    u8 *WrBuff = WriteBuffer;
    u16 PageSize = PAGE_SIZE;
    

    // Scrive pagina per pagina finché i byte di WrBuff sono minori della grandezza di una pagina
	while (DataSize >= PageSize) {
		Status = WriteFlash(WrAddr, WrBuff, PageSize);
		if (Status != XST_SUCCESS) {
			xil_printf("Scrittura pagina non riuscita\r\n");
			//Status = XBIR_ERROR_IMAGE_WRITE;
			goto END;
		}
		WrAddr += PageSize;
		WrBuff += PageSize;
		DataSize -= PageSize;
	}


	if (DataSize > 0U) {
		Status = WriteFlash(WrAddr, WrBuff, DataSize);
		if (Status != XST_SUCCESS) {
			xil_printf("Scrittura pagina non riuscita\r\n");
			goto END;
            //Status = XBIR_ERROR_IMAGE_WRITE;
		}
    }

    if (UploadImgInfo.UploadImg == IMG_A_ID) {
        WritePersistentRegister(REQUESTED_BOOT_IMG_A, IMG_A_BOOTABLE, IMG_B_BOOTABLE);
    }
    else {
        WritePersistentRegister(REQUESTED_BOOT_IMG_B, IMG_A_BOOTABLE, IMG_B_BOOTABLE);
    }

    xil_printf("Upload immagine completato con successo\r\n");


END:
	return Status;
}



int ReadImageSelectorRegister(ImageSelectorRegistersToRead ImageSelectorRegisters) {
	int Status = XST_FAILURE;

    u8 ReadBuffer[32] = {0U};
    u32 RegisterBuffer[8] = {0U};
    u32 Address = 0U;

    // Imposta l'indirizzo in base a quello richiesto
    if (ImageSelectorRegisters == IMAGE_SELECTOR_REGISTERS) {
    	Address = IMAGE_SELECTOR_REGISTERS_ADDR;
    }
    else {
    	Address = IMAGE_SELECTOR_REGISTERS_BCK_ADDR;
    }

    // Legge il registro dell'Image Status e mette tutto dentro ReadBuffer
    Status = ReadFlash(Address, ReadBuffer, sizeof(ReadBuffer));
    if (Status != XST_SUCCESS) {
		xil_printf("Lettura Image Selector Register non riuscita\r\n");
		//Status = XBIR_ERROR_IMAGE_WRITE;
		goto END;
	}


    // Facciamo la conversione da u8 a u32
    for (int i = 0; i < 8; i++) {
    RegisterBuffer[i] =
        ((u32)ReadBuffer[i * 4])       |
        ((u32)ReadBuffer[i * 4 + 1] << 8)  |
        ((u32)ReadBuffer[i * 4 + 2] << 16) |
        ((u32)ReadBuffer[i * 4 + 3] << 24);
    }
    

    // Assegnamo i valori a ogni elemento della struttura SysBootImgInfo
    memcpy(BootImgInfo.IdStr, ReadBuffer, 4);
    BootImgInfo.Ver               = RegisterBuffer[1];
    BootImgInfo.Len               = RegisterBuffer[2];
    BootImgInfo.Checksum          = RegisterBuffer[3];

    BootImgInfo.PersistentState.LastBootedImg   = (u8)(RegisterBuffer[4] & 0x000000FF);
    BootImgInfo.PersistentState.RequestedBootImg= (u8)((RegisterBuffer[4] >> 8) & 0x000000FF);
    BootImgInfo.PersistentState.ImgBBootable    = (u8)((RegisterBuffer[4] >> 16) & 0x000000FF);
    BootImgInfo.PersistentState.ImgABootable    = (u8)((RegisterBuffer[4] >> 24) & 0x000000FF);

    BootImgInfo.BootImgAOffset    = RegisterBuffer[5];
    BootImgInfo.BootImgBOffset    = RegisterBuffer[6];
    BootImgInfo.RecoveryImgOffset = RegisterBuffer[7];


    /*
    xil_printf("\r\nBoot Image Info:\r\n");
    
    xil_printf("  IdStr: %c%c%c%c\r\n",
    BootImgInfo.IdStr[0],
    BootImgInfo.IdStr[1],
    BootImgInfo.IdStr[2],
    BootImgInfo.IdStr[3]);
    
    xil_printf("  Version: 0x%08X\r\n", BootImgInfo.Ver);
    xil_printf("  Length: 0x%08X (%d bytes)\r\n", BootImgInfo.Len, BootImgInfo.Len);
    xil_printf("  Checksum: 0x%08X\r\n", BootImgInfo.Checksum);


    xil_printf("  Persistent State:\r\n");
    xil_printf("    LastBootedImg: %d\r\n", BootImgInfo.PersistentState.LastBootedImg);
    xil_printf("    RequestedBootImg: %d\r\n", BootImgInfo.PersistentState.RequestedBootImg);
    xil_printf("    ImgBBootable: %d\r\n", BootImgInfo.PersistentState.ImgBBootable);
    xil_printf("    ImgABootable: %d\r\n", BootImgInfo.PersistentState.ImgABootable);

    xil_printf("  Boot Image A Offset: 0x%08X\r\n", BootImgInfo.BootImgAOffset);
    xil_printf("  Boot Image B Offset: 0x%08X\r\n", BootImgInfo.BootImgBOffset);
    xil_printf("  Recovery Image Offset: 0x%08X\r\n", BootImgInfo.RecoveryImgOffset);
    */


END:
    return Status;
}



int WritePersistentRegister(RequestedBootImg RqstBootImg, Img_A_Status ImgASt, Img_B_Status ImgBSt) {
	int Status = XST_FAILURE;

    // Carica sulla struttura SysBootImgInfo i dati contenuti nel registro di backup
    ReadImageSelectorRegister(BACKUP_IMAGE_SELECTOR_REGISTERS);

    // Cancella il settore relativo all'Image Selector register
    SectorErase(IMAGE_SELECTOR_REGISTERS_ADDR);


    // Imposta i valori da scrivere nel registro persistente in base a quelli richiesti
    if ( RqstBootImg == REQUESTED_BOOT_IMG_A ) {
        BootImgInfo.PersistentState.RequestedBootImg = 0x0U;
    }
    else {
        BootImgInfo.PersistentState.RequestedBootImg = 0x01U;
    }

    if ( ImgASt == IMG_A_NON_BOOTABLE ) {
        BootImgInfo.PersistentState.ImgABootable = 0x00U;
    }
    else {
        BootImgInfo.PersistentState.ImgABootable = 0x01U;
    }

    if ( ImgBSt == IMG_B_NON_BOOTABLE ) {
        BootImgInfo.PersistentState.ImgBBootable = 0x00U;
    }
    else {
        BootImgInfo.PersistentState.ImgBBootable = 0x01U;
    }


    // Scrive tutto il registro dell'Image Selector
    Status = WriteFlash(IMAGE_SELECTOR_REGISTERS_ADDR, (u8*)&BootImgInfo, sizeof(BootImgInfo));
    if (Status != XST_SUCCESS) {
		xil_printf("Scrittura Image Selector Register non riuscita\r\n");
		//Status = XBIR_ERROR_IMAGE_WRITE;
		goto END;
	}

    //xil_printf("Image Selector Registers aggiornari\n\r");


END:
    return Status;
}



void PrintImageSelectorRegister (struct tcp_pcb *Tpcb){
    
    char WriteBuffer[1024];

    sprintf(WriteBuffer, 

        "\r\n\r\n---------- Boot Image Info ----------\r\n\r\nIdStr: %c%c%c%c\r\nVersion: 0x%08X\r\nLength: 0x%08X (%d bytes)\r\nChecksum: 0x%08X\r\n\r\nPersistent State:\r\nLastBootedImg: %d\r\nRequestedBootImg: %d\r\nImgBBootable: %d\r\nImgABootable: %d\r\n\r\nBoot Image A Offset: 0x%08X\r\nBoot Image B Offset: 0x%08X\r\nRecovery Image Offset: 0x%08X\r\n\r\n",

            BootImgInfo.IdStr[0], BootImgInfo.IdStr[1], BootImgInfo.IdStr[2], BootImgInfo.IdStr[3],
            BootImgInfo.Ver, BootImgInfo.Len, BootImgInfo.Len, BootImgInfo.Checksum,
            BootImgInfo.PersistentState.LastBootedImg, BootImgInfo.PersistentState.RequestedBootImg,
            BootImgInfo.PersistentState.ImgBBootable, BootImgInfo.PersistentState.ImgABootable,
            BootImgInfo.BootImgAOffset, BootImgInfo.BootImgBOffset, BootImgInfo.RecoveryImgOffset);

    
    tcp_write(Tpcb, WriteBuffer, strlen(WriteBuffer), 1);

    
    return;
}



int ImageErase(struct tcp_pcb *Tpcb, BootImgID ImgID) {
    int Status = XST_FAILURE;

    u32 Address;

    // Imposta l'Address dell'immagine richiesta per l'erase
    if ( ImgID == IMG_A_ID ) {
        Address = IMAGE_A_OFFSET;
    }
    else {
        Address = IMAGE_B_OFFSET;
    }

    // Si fa un ciclo for per cancellare tutti i settori necessari per cancellare completamente l'immagine
    for( int SectorErased = 0; SectorErased < IMAGE_SIZE / SECTOR_SIZE; SectorErased++ ){

        Status = SectorErase(Address);
        if (Status != XST_SUCCESS) {
		//xil_printf("Erase del settore nr�%d non riuscita\r\n", SectorErased+1);
		//Status = XBIR_ERROR_IMAGE_WRITE;
		goto END;
	    }

        // Ogni volta si incrementa l'Address così si passa al settore successivo
        Address += SECTOR_SIZE;
        
    }

    if (ImgID == IMG_A_ID) {
        WritePersistentRegister(REQUESTED_BOOT_IMG_B, IMG_A_NON_BOOTABLE, IMG_B_BOOTABLE);
        UploadImgInfo.EraseStatusImgA = IMG_ERASED;
    }
    else {
        WritePersistentRegister(REQUESTED_BOOT_IMG_A, IMG_A_BOOTABLE, IMG_B_NON_BOOTABLE);
        UploadImgInfo.EraseStatusImgB = IMG_ERASED; 
    }

    xil_printf("\r\n Image erase completato con successo\r\n");

    
END:
    return Status;
}



int StartImgUpload(struct tcp_pcb *Tpcb, u32 Address, u32 ImgLength, u8 *ReqPayload, u16 ReqLen)
{
    int Status = XST_SUCCESS;
    ImageSize = ImgLength;


    u16 HeaderSize = TCP_POST_REQ_LENGHT + POST_REQUEST_LENGHT + IMAGE_SIZE_LENGHT + (2*DELIMITER_LENGHT);
    u8 *ImgData = ReqPayload + HeaderSize;
    u32 FirstChunkLen = ReqLen - HeaderSize;


    // Controlla che ci siano dati immagine all'interno del payload
    if (ReqLen <= HeaderSize) {
        xil_printf("Errore: payload troppo corto, nessun dato immagine presente\r\n");
        tcp_write(Tpcb, "\r\nErrore nell'Upload del primo pacchetto: NoImgInsidePayload\r\n", strlen("\r\nErrore nell'Upload del primo pacchetto: NoImgInsidePayload\r\n"), 1);
		tcp_output(Tpcb);
        Status = XST_FAILURE;
        goto END;
    }


    // Controlla che l'immagine sia stata precedentemente cancellata
    if ((Address == IMAGE_A_OFFSET) && (UploadImgInfo.EraseStatusImgA == IMG_NON_ERASED)) {
		xil_printf("Prima di fare l'Upload dell'Immagine fare l'erase della Memoria\r\n");
        tcp_write(Tpcb, "\r\nErrore nell'Upload del primo pacchetto: ImgANotErased\r\n", strlen("\r\nErrore nell'Upload del primo pacchetto: ImgANotErased\r\n"), 1);
        tcp_output(Tpcb);
        Status = XST_FAILURE;
		goto END;
	}
    else if (Address == IMAGE_A_OFFSET){
        UploadImgInfo.UploadImg = IMG_A_ID;
        UploadImgInfo.EraseStatusImgA = IMG_NON_ERASED;
    }

    if ((Address == IMAGE_B_OFFSET) && (UploadImgInfo.EraseStatusImgB == IMG_NON_ERASED)) {
		xil_printf("Prima di fare l'Upload dell'Immagine fare l'erase della Memoria\r\n");
        tcp_write(Tpcb, "\r\nErrore nell'Upload del primo pacchetto: ImgBNotErased\r\n", strlen("\r\nErrore nell'Upload del primo pacchetto: ImgBNotErased\r\n"), 1);
		tcp_output(Tpcb);
        Status = XST_FAILURE;
		goto END;
	}
    else if (Address == IMAGE_B_OFFSET){
        UploadImgInfo.UploadImg = IMG_B_ID;
        UploadImgInfo.EraseStatusImgB = IMG_NON_ERASED;
    }


    // Copia i dati immagine ricevuti nel buffer di trasferimento in DDR
    memcpy(TX_BUFFER_BASE, ImgData, FirstChunkLen);
    // Modifica l'indirizzo corrente e i byte rimanenti
    RemainingImgData = ImgLength - FirstChunkLen;
    CurrentAddress = FirstChunkLen;


END:	
    return Status;
}



// Processa il payload dei pacchetti successivi
int ProcessAdditionalPayload(struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen)
{
    int Status = XST_SUCCESS;

    u32 ChunkLen;
    ImgDataStatus IsLast;


    // Determina la lunghezza del chunk da copiare
    if (ReqLen > RemainingImgData) {
        ChunkLen = RemainingImgData;
    } else {
        ChunkLen = ReqLen;
    }


    // Determina se è l'ultimo chunk
    if (ChunkLen == RemainingImgData) {
        IsLast = LAST_DATA_CHUNK;
    } else {
        IsLast = PARTIAL_DATA_CHUNK;
    }


    // Copia i dati ricevuti nel buffer di trasferimento in DDR
    memcpy((u8 *)(TX_BUFFER_BASE + CurrentAddress), ReqPayload, ChunkLen);

    // Modifica l'indirizzo corrente e i byte rimanenti
    CurrentAddress += ChunkLen;
    RemainingImgData -= ChunkLen;


    // Se è l'ultimo chunk avvia la scrittura dell'immagine
    if (IsLast == LAST_DATA_CHUNK) {
        xil_printf("\r\nScrittura in DDR completata, inizio trasferimento DMA\n\r");
        
        Status = DMA_Transfer(ImageSize);
        if (Status != XST_SUCCESS) {
            tcp_write(Tpcb, "Errore nel trasferimento DMA\r\n", strlen("Errore nel trasferimento DMA\r\n"), 1);     
            xil_printf("Errore nel trasferimento DMA\r\n");
            goto END;
        }

        xil_printf("\r\nTrasferimento DMA completato, inizio scrittura in QSPI \n\r");

        Status = ImageWrite(IMAGE_A_OFFSET, (u8 *)RX_BUFFER_BASE, ImageSize);
        if (Status != XST_SUCCESS) {
            tcp_write(Tpcb, "Errore nella scrittura dell'immagine completa da DDR a QSPI\r\n", strlen("Errore nella scrittura dell'immagine completa da DDR a QSPI\r\n"), 1);     
            xil_printf("Errore nella scrittura del blocco da DDR a QSPI\r\n");
            goto END;
        }
        
        tcp_write(Tpcb, "\r\nUpload dell'immagine completato\r\n", strlen("\r\nUpload dell'immagine completato\r\n"), 1);
    }

    
END:    
    return Status;
}
