#include <stdio.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "tcp.h"
#include "server.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "sys.h"

//da rimuovere
#include "qspi.h"



static TcpArgStruct TcpArgArray[TCP_MAX_SESSION];
static int TcpArgStructIndex = {0U};



// Dichiarazioni di funzioni da mettere all'inizio del file
static ReqPayloadType DecodeReq(char *ReqPayload, u16 ReqLen);
static ReqPayloadType DecodeReq (char *ReqPayload, u16 ReqLen);
static int ProcessGetReq (struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen);
static int ProcessPostReq(struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen);
static void ExtractFileName(char *ReqPayload, u16 ReqLen, char *FileName, u8 FileNameSize);
static void ExtractImgLength(char *ReqPayload, u16 ReqLen, u32 *ImgLength);



TcpArgStruct* AllocTcpArg(void) {
    TcpArgStruct *TcpArg = &(TcpArgArray[TcpArgStructIndex]);
    TcpArgStructIndex++;
    if (TcpArgStructIndex == TCP_MAX_SESSION)
        TcpArgStructIndex = 0;

    TcpArg->ConnID = TcpArgStructIndex;
    TcpArg->PktCount = 0;
    TcpArg->TotalBytes = 0;

    return TcpArg;
}



void ResetTcpArgCounters(struct tcp_pcb *Tpcb) {
    TcpArgStruct *TcpArg = (TcpArgStruct *)Tpcb->callback_arg;
    if (TcpArg != NULL) {
        TcpArg->PktCount = 0;
        TcpArg->TotalBytes = 0;
        //xil_printf("Connessione %u: contatori azzerati\n\r", TcpArg->ConnID);
    }
}



// Gestisce le varie richieste TCP in arrivo distinguendole in base all'header
int ProcessReq (struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen)
{
	// Variabile interna alla funzione per tenere traccia dello stato
	int Status = XST_SUCCESS;
	// Richiama la struttura che definisce i tipi di richiesta (GUARDA SU COPILOT)
	ReqPayloadType ReqType;


	// Analizza il tipo di richiesta e lo assegna a ReqType, (DA DEFFINIRE I TIPI DI RICHIESTA)
	ReqType = DecodeReq((char *)ReqPayload, ReqLen);


	// Con questo switch vengono chiamate le funzioni che gestiscono i vari tipi di richiesta (DA DEFINIRE I TIPI DI RICHIESTA)
	switch (ReqType) {

	// Sia GET che HEAD vengono gestiti dalla stessa funzione ProcessGetReq
	case TCP_GET:
	case TCP_HEAD:
		//xil_printf("Richiesta GET/HEAD ricevuta\r\n");
		Status = ProcessGetReq(Tpcb, ReqPayload, ReqLen);
		break;


	// POST viene gestito dalla funzione ProcessPostReq
	case TCP_POST:
		//xil_printf("Richiesta POST ricevuta\r\n");
		Status = ProcessPostReq(Tpcb, ReqPayload, ReqLen);
		break;


	// Se non riconosce il tipo di richiesta stampa un messaggio di errore
	default:
		//xil_printf("Non � stata riconosciuta alcuna richiesta GET/HEAD/POST\r\n");
		tcp_write(Tpcb, "\r\nErrore nella lettura della richiesta: UnknownRequest\n\r", strlen("\r\nErrore nella lettura della richiesta: UnknownRequest\n\r"), 1);
        tcp_output(Tpcb);
		Status = XST_FAILURE;
		break;
	}


	return Status;
}



static ReqPayloadType DecodeReq (char *ReqPayload, u16 ReqLen)
{
	ReqPayloadType ReqType = TCP_UNKNOWN;
	// Stringhe che identificano i vari tipi di richiesta (VALUTA SE INSERIRE DELIMITATORI "\n" O SIMILI, "\n" → ASCII Dec: 10 → Hex: 0x0A)
	char *GetReqStr = "GET";	 // "GET" -> ASCII Hex: 0x47 0x45 0x54          "GET\n" -> ASCII Hex: 0x47 0x45 0x54 0x0A
	char *PostReqStr = "POST";   // "POST" -> ASCII Hex: 0x50 0x4F 0x53 0x54    "POST\n" -> ASCII Hex: 0x50 0x4F 0x53 0x54 0x0A
	char *HeadReqStr = "HEAD";   // "HEAD" -> ASCII Hex: 0x48 0x45 0x41 0x44    "HEAD\n" -> ASCII Hex: 0x48 0x45 0x41 0x44 0x0A

	// Confronta il payload ricevuto con le stringhe che identificano i vari tipi di richiesta
	if (strncmp(ReqPayload, GetReqStr, strlen(GetReqStr)) == 0) {
		ReqType = TCP_GET;
	}
	else if (strncmp(ReqPayload, HeadReqStr, strlen(HeadReqStr)) == 0) {
		ReqType = TCP_HEAD;
	}
	else if (strncmp(ReqPayload, PostReqStr, strlen(PostReqStr)) == 0) {
		ReqType = TCP_POST;
	}
	else {
		ReqType = TCP_UNKNOWN;
	}


	return ReqType;
}



static int ProcessGetReq (struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen)
{
	int Status = XST_SUCCESS;
	char FileName[MAX_TCP_REQUEST_LENGHT] = "";
	

	// Qui viene estratto il nome del file dalla richiesta TCP
	ExtractFileName((char *)ReqPayload, ReqLen, FileName, MAX_TCP_REQUEST_LENGHT - 1U);


	// Da qui gestisce ogni tipo di richiesta TCP

	// Restituisce lo stato delle immagini di boot
	if (strncmp(FileName, "boot_img_status", strlen("boot_img_status")) == 0) {
		//xil_printf("Richiesta di stato dell'immagine di boot\r\n");
		Status = ReadImageSelectorRegister(IMAGE_SELECTOR_REGISTERS);
		if ( Status != XST_SUCCESS ) {
			tcp_write(Tpcb, "\r\nErrore nella lettura dei registri dell'Image Selector\n\r", strlen("\r\nErrore nella lettura dei registri dell'Image Selector\n\r"), 1);
        	//xil_printf("Errore nella lettura dei registri\r\n");
			goto END;
    	}
		PrintImageSelectorRegister(Tpcb);
	}

	// Cancella l'immagine A
	else if (strncmp(FileName, "flash_erase_imgA", strlen("flash_erase_imgA")) == 0) {
		//xil_printf("Richiesta di cancellazione dell'immagine A\r\n");
		//tcp_write(Tpcb, "\r\n\r\nErase dell'immagine A iniziato\n\r", strlen("\r\n\r\nErase dell'immagine A iniziato\n\r"), 1);
		Status = ImageErase(Tpcb, IMG_A_ID);
		if ( Status != XST_SUCCESS ) {
        	xil_printf("Errore nell'erase dell'immagine A\r\n");
			tcp_write(Tpcb, "\r\nErrore nell'erase dell'immagine A\r\n", strlen("\r\nErrore nell'erase dell'immagine A\r\n"), 1);
			goto END;
    	}
		tcp_write(Tpcb, "\r\n\r\nErase dell'immagine A completato con successo\n\r\r\n", strlen("\r\n\r\nErase dell'immagine A completato con successo\n\r\r\n"), 1);
	}

	// Cancella l'immagine B
	else if (strncmp(FileName, "flash_erase_imgB", strlen("flash_erase_imgB")) == 0) {
		//xil_printf("Richiesta di cancellazione dell'immagine B\r\n");
		//tcp_write(Tpcb, "\r\n\r\nErase dell'immagine B iniziato\n\r", strlen("\r\n\r\nErase dell'immagine B iniziato\n\r"), 1);
		Status = ImageErase(Tpcb, IMG_B_ID);
		if ( Status != XST_SUCCESS ) {
        	tcp_write(Tpcb, "\r\nErrore nell'erase dell'immagine B\r\n", strlen("\r\nErrore nell'erase dell'immagine B\r\n"), 1);
			xil_printf("Errore nell'erase dell'immagine A\r\n");
			goto END;
    	}
		tcp_write(Tpcb, "\r\n\r\nErase dell'immagine B completato con successo\n\r\r\n", strlen("\r\n\r\nErase dell'immagine B completato con successo\n\r\r\n"), 1);
	}


    // Qui vengono gestiti gli errori

	else if (strncmp(FileName, "RequestTooLong", strlen("RequestTooLong")) == 0) {
		xil_printf("Richiesta troppo lunga\r\n");
		// Gestione dell'errore da mettere 
		tcp_write(Tpcb, "\r\n\r\nErrore nella lettura della richiesta: RequestTooLong\n\r", strlen("\r\nErrore nella lettura della richiesta: RequestTooLong\n\r"), 1);
	}

	else {
		xil_printf("Richiesta non riconosciuta\r\n");
		// Gestione dell'errore da mettere 
		tcp_write(Tpcb, "\r\n\r\nErrore nella lettura della richiesta: UnknownRequest\n\r", strlen("\r\nErrore nella lettura della richiesta: UnknownRequest\n\r"), 1);
	}


	ResetTcpArgCounters(Tpcb);


END:
	return Status;
}



static int ProcessPostReq(struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen) {
	int Status = XST_SUCCESS;
	char FileName[MAX_TCP_REQUEST_LENGHT] = "";
	u32 ImgLenght = {0U};
	u32 Address = {0U};
	char WriteBuffer[128];


	// Qui viene estratto il nome del file dalla richiesta TCP, (MODIFICA IL FATTO CHE I REGISTRI VENGONO MODIFICATI PRIMA DI VEDERE LA LUNGHEZZA)
	ExtractFileName((char *)ReqPayload, ReqLen, FileName, MAX_TCP_REQUEST_LENGHT - 1U);

	if (strncmp(FileName, "Upload_Img_A", strlen("Upload_Img_A")) == 0) {
		tcp_write(Tpcb, "\r\n\r\nInizio dell'Upload dell'Immagine A\n\r", strlen("\r\n\r\nInizio dell'Upload dell'Immagine A\n\r"), 1);
		//xil_printf("Riconosciuta richiesta di Upload dell'immagine A\r\n");
		Address = IMAGE_A_OFFSET;
		WritePersistentRegister(REQUESTED_BOOT_IMG_A, IMG_A_NON_BOOTABLE, IMG_B_BOOTABLE);
	}

	else if (strncmp(FileName, "Upload_Img_B", strlen("Upload_Img_B")) == 0) {
		tcp_write(Tpcb, "\r\n\r\nInizio dell'Upload dell'Immagine B\n\r", strlen("\r\n\r\nInizio dell'Upload dell'Immagine B\n\r"), 1);
		//xil_printf("Riconosciuta richiesta di Upload dell'immagine B\r\n");
		Address = IMAGE_B_OFFSET;
		WritePersistentRegister(REQUESTED_BOOT_IMG_B, IMG_A_BOOTABLE, IMG_B_NON_BOOTABLE);
	}

	else {
		tcp_write(Tpcb, "\r\nErrore nella lettura della richiesta: UnknownRequest\n\r", strlen("\r\nErrore nella lettura della richiesta: UnknownRequest\n\r"), 1);
		//xil_printf("Non � stata riconosciuta nessuna richiesta di POST\r\n");
		Status = XST_FAILURE;
		goto END;
	}


	ExtractImgLength((char *)ReqPayload, ReqLen, &ImgLenght);

	if (ImgLenght == 0U || ImgLenght > IMAGE_SIZE) {
		tcp_write(Tpcb, "\r\nErrore nella lettura della lunghezza: UnvalidLenght\n\r", strlen("\r\nErrore nella lettura della lunghezza: UnvalidLenght\n\r"), 1);
		//xil_printf("Lunghezza immagine non valida\r\n");
		Status = XST_FAILURE;
		goto END;
	}
	else {
		//xil_printf("Lunghezza immagine %d Bytes\r\n", ImgLenght);
		sprintf(WriteBuffer, "\r\nLunghezza immagine %d bytes\r\n", ImgLenght);
		tcp_write(Tpcb, WriteBuffer, strlen(WriteBuffer), 1);
	}


	Status = StartImgUpload(Tpcb ,Address, ImgLenght, ReqPayload, ReqLen);


END:
	return Status;
}



static void ExtractFileName(char *ReqPayload, u16 ReqLen, char *FileName, u8 FileNameSize)
{
	char *Start, *End;
	u16 Offset;

	// Qui controlla se la richiesta � di tipo GET o POST e imposta l'offset di conseguenza
	if (strncmp(ReqPayload, "GET", 3) == 0)
		Offset = 4; // 3 Byte "GET" + 1 Byte "\n" 
	else {
		Offset = 5; // 4 Byte "HEAD"/"POST" + 1 Byte "\n" 
	}


	// Puntatore all'inizio del nome del file (ReqPayload punta al payload TCP)
	Start = ReqPayload + Offset;  
	

	// Avanza l'offset finché non incontra uno "\n" (che indica la fine del nome del file), "\n" -> ASCII Hex: 0x0A
	while (ReqPayload[Offset] != '/' && Offset < ReqLen) {
		Offset++;
	}


	// Puntatore all'ultimo carattere del nome del file
	End = ReqPayload + Offset - 1;


	// Se la lunghezza del nome del file � maggiore della dimensione massima consentita
	if ((Offset > ReqLen) || ((End - Start) > FileNameSize)) {
		*End = 0;
		strcpy(FileName, "RequestTooLong");
		goto END;
	}


	// Copia un numero di caratteri "End - Start + 1U" a partire da "Start" in "FileName"
	strncpy(FileName, Start, End - Start + 1U);
	// Serve per rendere la stringa "FileName" terminata correttamente
	FileName[End - Start + 1U] = 0U;

	//xil_printf("Nome del file richiesto: %s\r\n", FileName);


END:
	return;
}



static void ExtractImgLength(char *ReqPayload, u16 ReqLen, u32 *ImgLength)
{
    u16 Offset = 0;
    u8 NewlineCount = 0;
    char FileName[IMAGE_SIZE_LENGHT + 1U] = "";
    char *Start, *End;


    // Avanza fino al secondo '/'
    while (Offset < ReqLen && NewlineCount < 2) {
        if (ReqPayload[Offset] == 0x2F) { // '/' in ASCII
            NewlineCount++;
        }
        Offset++;
    }

	if ((Offset + IMAGE_SIZE_LENGHT) > ReqLen) {
        *ImgLength = 0xFFFFFFFF; // Valore di errore
        xil_printf("Messaggio troppo corto per contenere ImgLength\r\n");
        goto END;
    }


    Start = ReqPayload + Offset;

    End = ReqPayload + Offset + IMAGE_SIZE_LENGHT - 1;


    strncpy(FileName, Start, End - Start + 1U);
    FileName[IMAGE_SIZE_LENGHT] = 0U;


    *ImgLength = (u32)strtoul(FileName, NULL, 16);
   
    xil_printf("Lunghezza immagine estratta: 0x%08X (%lu byte)\r\n", *ImgLength, *ImgLength);


END:
	return;
}
