#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sys.h"
#include "xstatus.h"
#include "qspi.h"
#include "err.h"



static u8 DataBuffer[200000U];
static SysBootImgInfo BootImgInfo;



int ImageWrite(u32 DstAddress, u8 *WriteBuffer, u32 ByteCount, Xbir_ImgDataStatus IsLast) { // Xbir_ImgDataStatus (DA MODIFICARE)
    int Status = XST_FAILURE;

    u32 DataSize = ByteCount;
	u32 WrAddr = DstAddress;
    u8 *WrBuff = WriteBuffer;
    static u32 PrevPendingDataLen = 0U;
    u16 PageSize = PAGE_SIZE;


    // Se ci sono dei dati che non sono stati scritti la volta precedente li aggiunge al DataBuffer
    if (PrevPendingDataLen > 0U) {
		if ((ByteCount + PrevPendingDataLen) > sizeof(DataBuffer)) {
			xil_printf("File troppo grande\r\n");
			//Status = XBIR_ERROR_IMAGE_SIZE;
			goto END;
		}
		memcpy (&DataBuffer[PrevPendingDataLen], WriteBuffer, ByteCount);
		DataSize += PrevPendingDataLen;
		WrAddr -= PrevPendingDataLen;
		WrBuff = DataBuffer;
	}


    // Se la quantità di dati da scrivere è maggiore della grandezza di una pagina allora scrive una pagina
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


    // Arrivati qui ci sono gli ultimi dati rimanenti, se il pacchetto era intermedio allora li copia sul buffer e si salva l'offset, se era l'ultimo scrive anche quelli
	if (XBIR_SYS_PARTIAL_DATA_CHUNK == IsLast) {
		memcpy(DataBuffer, WrBuff, DataSize);
		PrevPendingDataLen = DataSize;
	}
	else {
		PrevPendingDataLen = 0U;
		if (DataSize > 0U) {
			Status = WriteFlash(WrAddr, WrBuff, DataSize);
			if (Status != XST_SUCCESS) {
				xil_printf("Scrittura pagina non riuscita\r\n");
				//Status = XBIR_ERROR_IMAGE_WRITE;
			}
		}
	}


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
    */

    xil_printf("  Persistent State:\r\n");
    xil_printf("    LastBootedImg: %d\r\n", BootImgInfo.PersistentState.LastBootedImg);
    xil_printf("    RequestedBootImg: %d\r\n", BootImgInfo.PersistentState.RequestedBootImg);
    xil_printf("    ImgBBootable: %d\r\n", BootImgInfo.PersistentState.ImgBBootable);
    xil_printf("    ImgABootable: %d\r\n", BootImgInfo.PersistentState.ImgABootable);

    /*
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

    if ( ImgASt == IMG_A_NON_BOOTABLE ) {
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


END:
    return Status;
}



int ImageErase(BootImgID ImgID) {
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
		xil_printf("Erase del settore nr�%d non riuscita\r\n", SectorErased+1);
		//Status = XBIR_ERROR_IMAGE_WRITE;
		goto END;
	    }

        // Ogni volta si incrementa l'Address così si passa al settore successivo
        Address += SECTOR_SIZE;
        
    }

    
END:
    return Status;
}
