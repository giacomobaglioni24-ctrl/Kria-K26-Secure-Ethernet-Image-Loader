#include <stdio.h>
#include "xil_printf.h"
#include "platform.h"
#include "qspi.h"
#include "sys.h"
#include "xstatus.h"


int main()
{    
    /*
    u32 Address = 0x022A0000U;
    u8 ReadBuffer[1024U] = {0U};
    u8 WriteBuffer[1000U] = {0U};
    */

    init_platform();

    if ( XQspi_Init() != XST_SUCCESS ) {
        xil_printf("Error: %d\r\n", XQspi_Init());
    }
    xil_printf("QSPI init done\r\n");

    // Leva il commento per stampare i dati letti
    /*
    for (int i = 0; i < ByteCount; i = i+4) {
        xil_printf("Byte %d: 0x%02X%02X%02X%02X\r\n", i, ReadBuffer[i], ReadBuffer[i+1], ReadBuffer[i+2], ReadBuffer[i+3]);
    }  
    */

    //WritePersistentRegister(REQUESTED_BOOT_IMG_A, IMG_A_BOOTABLE, IMG_B_BOOTABLE);

    ReadImageSelectorRegister(IMAGE_SELECTOR_REGISTERS);
    ReadImageSelectorRegister(BACKUP_IMAGE_SELECTOR_REGISTERS);

    //ImageErase(IMG_A_ID);

    cleanup_platform();

    return 0;
}
