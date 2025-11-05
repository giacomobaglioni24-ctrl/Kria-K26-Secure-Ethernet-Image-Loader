#ifndef TCP_H
#define TCP_H

#include "xil_types.h"
#include "lwip/tcp.h"



// Definizioni
#define MAX_TCP_REQUEST_LENGHT (25U) // Lunghezza massima del nome del file richiesto tramite TCP, (4 Byte richiesta + 1 Byte delimitatore "\n", 24 Byte nome file + 1 Byte delimitatore "\n", 4 Byte delimitatore) 
#define TCP_POST_REQ_LENGHT (4U)
#define POST_REQUEST_LENGHT (12U)
#define IMAGE_SIZE_LENGHT (8U)
#define DELIMITER_LENGHT (1U)


// Tipi di richiesta TCP
typedef enum {
	TCP_GET,
	TCP_POST,
	TCP_HEAD,
	TCP_UNKNOWN
} ReqPayloadType;



typedef struct {
    u32 ConnID;
    u32 PktCount;
    u32 TotalBytes;
} TcpArgStruct;



int ProcessReq (struct tcp_pcb *Tpcb, u8 *ReqPayload, u16 ReqLen);
void ResetTcpArgCounters(struct tcp_pcb *Tpcb);
TcpArgStruct* AllocTcpArg(void);

#endif // TCP_H
