Descrizione dell'Applicazione:


Il sistema riceve i dati cifrati tramite Ethernet, gestiti dal Processing System (PS) che li memorizza in DDR4. Successivamente, un AXI DMA 
trasferisce i dati alla Programmable Logic, dove un IP custom esegue la decrittazione AES128 in modalità CTR attraverso un’interfaccia AXI4-Stream. 
I dati decrittati vengono riportati in DDR4 tramite lo stesso DMA e infine il PS li scrive nella QSPI Flash, completando il processo di
aggiornamento sicuro delle immagini di boot.





Organizzazione delle cartelle:


	- Python: Sono presenti tutti gli script python per leggere, criptare e incapsulare i file

	- Vitis: Sono presenti l'applicazione con tutti i suoi Driver (QSPI, DMA, TCP) e la Platform su cui è stata utilizzata

	- Vivado: Sono presenti tutti i file di codice del custom IP "AXI4-Stream AES128 CTR Decrypter" con il relativo Test Bench
		  e le configurazioni dell'IP, è anche presente un immagine del BD con tutti i suoi IP contenuti e il suo file .xsa





Istruzioni per l'uso degli script Python:


	- print_hex.py: Utilizzabile per la lettura di file di qualsiasi estensione in esadecimale, restituisce l'output sul terminale

			python print_hex.py -i "Nome File da leggere"

	- encrypt.py: Utilizzabile per criptare un file di qualsiasi estensione utilizzando il protocollo AES-128 CTR scegliendo la chiave
		      simmetrica (Il valore del counter parte da 1 ma si può cambiare modificando il codice). Come input prende il file
		      da Criptare e la chiave e restituisce l'output in un altro file a parte e il "Nonce" da utilizzare per la decriptazione
		      sul terminale

		      python encrypt.py -i BOOT.bin -o Image.bin -k 00112233445566778899AABBCCDDEEFF
	
		      (La chiave contenuta nel file .xsa è 0x7C9A3F1E4B2D8A6F0E5C1D7B3A9F4E2C)

	- encapsulate.py: Utilizzabile per incapsulare una Richiesta, un Nonce e un File in un unico file, calcola in automatico la lunghezza
			  del file Image.bin + 16 byte di Nonce in HEX e li mette sotto forma di 8 caratteri ASCII dopo la richiesta

			  python .\encapsulate.py -p POST/Upload_Img_A/ -x 00112233445566778899AABBCCDDEEFF -i Image.bin -o fullmessage.bin





Istruzioni per l'uso dell'applicazione:


All'avvio della scheda, con l'immagine caricata in QSPI, passeranno alcuni secondi e poi sarà possibile connettersi al server TCP come client


L'IP e la porta sono i seguenti: 192.168.1.10 1234		(Possono essere cambiati in "network.c" e si può anche aggiungere un Gateway)


Una volta connessi potranno essere inviate le seguenti richieste:

	- GET/boot_img_status/		Restituisce il contenuto dei registri dell'Image Selector

	- GET/flash_erase_imgA/		Avvia l'erase dell'immagine A con notifica alla fine

	- GET/flash_erase_imgB/		Avvia l'erase dell'immagine B con notifica alla fine

	- POST/Upload_Img_A/XXXXXXXXBOOT.bin	Avvia il caricamento del file "BOOT.bin" all'offset dell'immagine A, il campo "XXXXXXXX" sono 
						8 caratteri ASCII che vengono interpretati come 8 caratteri HEX e indicano la lunghezza 
						esclusivamente dell'immagine, si riceve una notifica una volta finito l'Upload 

	- POST/Upload_Img_B/XXXXXXXXBOOT.bin	Avvia il caricamento del file "BOOT.bin" all'offset dell'immagine B, il campo "XXXXXXXX" sono 
						8 caratteri ASCII che vengono interpretati come 8 caratteri HEX e indicano la lunghezza 
						esclusivamente dell'immagine, si riceve una notifica una volta finito l'Upload 


Il file "BOOT.bin"che deve essere mandato deve avere la seguente struttura: 	Richiesta|Nonce|ImmagineCriptata

Si può sfruttare lo script python "encapsulate.py" (Il carattere "|" non deve essere incluso e non c'è nessuno spazio tra i vari componenti del file)

