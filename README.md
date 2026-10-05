## Project Overview

The system receives encrypted data via Ethernet, managed by the Processing System (PS), which temporarily stores it in the DDR4 memory. Subsequently, an AXI DMA transfers the data to the Programmable Logic (PL), where a custom IP performs AES-128 CTR decryption using an AXI4-Stream interface. The decrypted data is then routed back to the DDR4 memory via the same DMA. Finally, the PS writes the data into the QSPI Flash, successfully completing the secure boot image update process.

## Repository Structure

* **`Python/`**: Contains all Python scripts used for reading, encrypting, and encapsulating files.
* **`Vitis/`**: Includes the baremetal application along with its peripheral drivers (QSPI, DMA, TCP) and the hardware platform description.
* **`Vivado/`**: Contains the source code for the custom "AXI4-Stream AES128 CTR Decrypter" IP, its testbench, and the IP configurations. It also includes an image of the Block Design (BD) with all instantiated IPs and the exported `.xsa` hardware handoff file.

## Python Scripts Usage

* **`print_hex.py`**
Reads a file of any extension and prints its hexadecimal representation to the terminal.
```bash
python print_hex.py -i "<File_to_read>"

```


* **`encrypt.py`**
Encrypts a file of any extension using the AES-128 CTR protocol with a user-defined symmetric key (the counter value starts at 1, but can be modified directly in the source code). It takes the input file and the key as arguments, outputs the encrypted file, and prints the "Nonce" required for decryption to the terminal.
```bash
python encrypt.py -i BOOT.bin -o Image.bin -k 00112233445566778899AABBCCDDEEFF

```


*(Note: The default key embedded in the `.xsa` file is `0x7C9A3F1E4B2D8A6F0E5C1D7B3A9F4E2C`)*
* **`encapsulate.py`**
Encapsulates a Request, a Nonce, and a File into a single binary file. It automatically calculates the payload length (`Image.bin` + 16 bytes of Nonce) in HEX format and appends it as an 8-character ASCII string immediately following the request header.
```bash
python encapsulate.py -p POST/Upload_Img_A/ -x 00112233445566778899AABBCCDDEEFF -i Image.bin -o fullmessage.bin

```



## Application Usage (TCP Server)

Upon board boot-up from the QSPI flash image, the system initializes for a few seconds before the TCP server becomes available for client connections.

**Default IP and Port:** `192.168.1.10 : 1234`
*(These parameters, as well as an optional Gateway, can be modified in `network.c`)*

Once connected, the client can send the following requests:

* `GET/boot_img_status/`
Returns the current contents of the Image Selector registers.
* `GET/flash_erase_imgA/`
Initiates the erase process for Image A and sends a notification upon completion.
* `GET/flash_erase_imgB/`
Initiates the erase process for Image B and sends a notification upon completion.
* `POST/Upload_Img_A/XXXXXXXXBOOT.bin`
Starts uploading the `BOOT.bin` file to the Image A offset. The `XXXXXXXX` field consists of 8 ASCII characters (interpreted as HEX) indicating the exact length of the image payload. A completion notification is sent once the upload finishes.
* `POST/Upload_Img_B/XXXXXXXXBOOT.bin`
Starts uploading the `BOOT.bin` file to the Image B offset. It behaves exactly like the Image A upload, using the same 8-character ASCII length field structure.

**Payload Formatting:**
The `BOOT.bin` file sent to the server must strictly follow this internal structure:
`Request` `Nonce` `EncryptedImage`

You can use the `encapsulate.py` script to generate this file automatically. *(Note: there are no spaces or separation characters like "|" between the various components of the file; they must be strictly contiguous).*
