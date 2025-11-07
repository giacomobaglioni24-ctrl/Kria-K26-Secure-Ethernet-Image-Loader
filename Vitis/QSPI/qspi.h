
#ifndef QSPI_H
#define QSPI_H

#include "xil_types.h"


// Comandi di lettura, scrittura ed erase
#define QUAD_READ_CMD_4BYTE (0x6CU)
#define QUAD_INPUT_FAST_PROGRAM_4BYTE (0x34U) 
#define SECTOR_ERASE_4BYTE (0xDCU) 					// 64KB;  65536 Byte;  0x10000 
#define SUBSECTOR_ERASE_4KB_4BYTE (0x21U)			// 4KB;   4096 Byte;   0x1000

// Comandi da utilizzare per leggere i registri di stato e volatili
#define READ_STATUS_CMD		(0x05U)
#define READ_FLAG_STATUS_CMD 	(0x70U)
#define READ_VOLATILE_CONFIGURATION_CMD 	(0x85U)
#define READ_ENHANCED_VOLATILE_CONFIGURATION_CMD  	(0x65U)
#define CLEAR_FLAG_STATUS_CMD 	(0x50U)

// Comandi per abilitar e disabilitare il WE
#define WRITE_ENABLE_CMD	(0x06U)
#define WRITE_DISABLE_CMD	(0x04U)

// Struttura messaggio da inviare con il polled transfer
#define COMMAND_OFFSET		(0U) // Offset del comando
#define ADDRESS_1_OFFSET	(1U) // MSB dell'address
#define ADDRESS_2_OFFSET	(2U) // Byte in mezzo all'Address
#define ADDRESS_3_OFFSET	(3U) // Byte in mezzo all'Address
#define ADDRESS_4_OFFSET	(4U) // LSB dell'address
#define DUMMY_CLOCKS		(8U)   // Numero di dummy bytes da mandare quando si esegue QUAD_READ_CMD_4BYTE

// Comandi di entrata e uscita dal 4Byte addressing mode
#define ENTER_4B_ADDR_MODE	(0xB7U)
#define EXIT_4B_ADDR_MODE	(0xE9U)

// Il limite di byte trasferibili da un singolo DMA (512MB)
#define DMA_DATA_TRAN_SIZE	(0x20000000U)

// Dimensione di una pagina (256B)
#define PAGE_SIZE (256U)


// Enum per abilitare o disabilitare il clear dello Status Flag register 
typedef enum {
	CLEAR_DISABLE = 0,
	CLEAR_ENABLE
} ClearStatusFlag;



// Funzioni all'interno di qspi.c
int XQspi_Init ();
int ReadFlash(u32 SrcAddress, u8 *ReadBuffer, u32 ByteCount);
int WriteFlash(u32 DstAddress, u8 *ReadBuffer, u32 ByteCount);
int SectorErase(u32 Address);

#endif // QSPI_H


