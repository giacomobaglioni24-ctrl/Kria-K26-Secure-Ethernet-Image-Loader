#ifndef SYS_H
#define SYS_H

#include "xil_types.h"


// Address Image Selector Registers
#define IMAGE_SELECTOR_REGISTERS_ADDR (0x00100000U)
#define PERSISTENT_REGISTER_ADDR (0x00100010U)
#define IMAGE_SELECTOR_REGISTERS_BCK_ADDR (0x00120000U)
#define PERSISTENT_REGISTER_BCK_ADDR (0x00120010U)

// Dimensione immagine
#define IMAGE_SIZE (0xD00000U)
#define SECTOR_SIZE (0x10000U)

// Offset immagini
#define IMAGE_A_OFFSET (0x00200000U)
#define IMAGE_B_OFFSET (0x00F80000U)



// Struttura per il Persistent Register
typedef struct {
	u8 LastBootedImg;
	u8 RequestedBootImg;
	u8 ImgBBootable;
	u8 ImgABootable;
} SysPersistentState;

// Struttura per gli Image Selector Registers
typedef struct {
	u8 IdStr[4U];
	u32 Ver;
	u32 Len;
	u32 Checksum;
	SysPersistentState PersistentState;
	u32 BootImgAOffset;
	u32 BootImgBOffset;
	u32 RecoveryImgOffset;
} SysBootImgInfo;



// Enum per l'ID dell'immagine
typedef enum {
	IMG_A_ID = 0,
	IMG_B_ID, /**< 1 */
} BootImgID;

// Enum per l'immagine richiesta
typedef enum {
	REQUESTED_BOOT_IMG_A = 0,
	REQUESTED_BOOT_IMG_B, /**< 1 */
} RequestedBootImg;

// Enum per lo stato dell'immagine A
typedef enum {
	IMG_A_NON_BOOTABLE = 0,
	IMG_A_BOOTABLE, /**< 1 */
} Img_A_Status;

// Enum per lo stato dell'immagine B
typedef enum {
	IMG_B_NON_BOOTABLE = 0,
	IMG_B_BOOTABLE, /**< 1 */
} Img_B_Status;

// Enum per comunicare alla funzione ReadPersistentRegister quale dei due registri leggere
typedef enum {
	IMAGE_SELECTOR_REGISTERS = 0,
	BACKUP_IMAGE_SELECTOR_REGISTERS,
} ImageSelectorRegistersToRead;



// Enum per definire se è un data chunk intermedio o finale
typedef enum {
	XBIR_SYS_PARTIAL_DATA_CHUNK = 0,
	XBIR_SYS_LAST_DATA_CHUNK
} Xbir_ImgDataStatus;



int ImageWrite(u32 DstAddress, u8 *WriteBuffer, u32 ByteCount, Xbir_ImgDataStatus IsLast);
int ReadImageSelectorRegister(ImageSelectorRegistersToRead ImageSelectorRegisters);
int WritePersistentRegister(RequestedBootImg RqstBootImg, Img_A_Status ImgASt, Img_B_Status ImgBSt);
int ImageErase(BootImgID ImgID);

#endif // SYS_H
