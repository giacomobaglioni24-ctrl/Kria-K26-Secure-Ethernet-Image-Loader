#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
encapsulate.py
Incapsulatore di pacchetti.

Calcola la dimensione *totale* del file di output (Header + Nonce + Payload)
e la inserisce come stringa ASCII-hex da 8 caratteri nell'header.

Header = [Prefisso Richiesta (ASCII)] + [Dimensione Totale (ASCII-Hex, 8 char)]
Pacchetto = [Header] + [Nonce (16 byte)] + [Payload (file)]
"""

import argparse
import sys
import os

# --- Costanti Fisse (come da tua specifica) ---
SIZE_FIELD_LEN = 8  # 8 caratteri ASCII per la dimensione (es. "00000047")
NONCE_LEN_HEX = 32  # 32 caratteri hex che diventano 16 byte
NONCE_LEN_BYTES = 16

def main():
    parser = argparse.ArgumentParser(description="Incapsulatore di pacchetti con dimensione totale")
    
    parser.add_argument("-p", "--prefix", 
                        dest="request_prefix",
                        help="Prefisso della richiesta in formato stringa ASCII (es. 'POST/Upload_img_A/')", 
                        required=True)
                        
    parser.add_argument("-x", "--hex", 
                        dest="hex_str",
                        help=f"Stringa esadecimale di {NONCE_LEN_BYTES} byte (Nonce) - ({NONCE_LEN_HEX} caratteri hex)", 
                        required=True)
                        
    parser.add_argument("-i", "--input", 
                        dest="input_file",
                        help="File di input (payload) da incapsulare", 
                        required=True)
                        
    parser.add_argument("-o", "--output", 
                        dest="output_file",
                        help="File di output dove salvare il pacchetto incapsulato", 
                        required=True)

    args = parser.parse_args()

    # --- Validazione Input ---
    
    # 1. Valida la stringa esadecimale (Nonce)
    if len(args.hex_str) != NONCE_LEN_HEX:
        print(f"Errore: La stringa esadecimale (--hex) deve essere esattamente di {NONCE_LEN_HEX} caratteri.")
        sys.exit(1)
        
    try:
        nonce_bytes = bytes.fromhex(args.hex_str)
    except ValueError:
        print(f"Errore: La stringa esadecimale (--hex) contiene caratteri non validi.")
        sys.exit(1)

    # 2. Valida il file di input
    if not os.path.exists(args.input_file):
        print(f"Errore: Il file di input '{args.input_file}' non è stato trovato.")
        sys.exit(1)

    try:
        # --- 1. Lettura e Calcolo Dimensioni ---
        
        # Leggi il payload per sapere la sua dimensione
        print(f"\nLettura del file di input '{args.input_file}'...")
        with open(args.input_file, 'rb') as f_in:
            payload_bytes = f_in.read()
            
        payload_size = len(payload_bytes)
        
        # Calcola la dimensione del prefisso
        prefix_bytes = args.request_prefix.encode('ascii')
        prefix_size = len(prefix_bytes)
        
        # Calcola la dimensione totale del file di output
        total_output_size = prefix_size + SIZE_FIELD_LEN + NONCE_LEN_BYTES + payload_size
        
        print(f"\n  --- Dimensioni Calcolate ---")
        print(f"  Prefisso Header: {prefix_size} bytes")
        print(f"  Campo Dimensione:  {SIZE_FIELD_LEN} bytes (fisso)")
        print(f"  Nonce:             {NONCE_LEN_BYTES} bytes (fisso)")
        print(f"  Payload:           {payload_size} bytes")
        print(f"  ---------------------------")
        print(f"\n  Dimensione Totale Calcolata: {total_output_size} bytes\n")

        # Controlla se la dimensione totale supera gli 8 caratteri hex (4 GByte)
        if total_output_size > 0xFFFFFFFF:
            print(f"Errore: La dimensione totale ({total_output_size} bytes) supera il massimo rappresentabile (4GB).")
            sys.exit(1)

        # --- 2. Costruzione Header ---
        
        # Formatta la dimensione totale come stringa hex ASCII di 8 caratteri
        size_field_ascii_str = f"{total_output_size:08X}"
        
        # Costruisci l'header completo
        full_request_str = args.request_prefix + size_field_ascii_str
        request_header_bytes = full_request_str.encode('ascii')
        
        print(f"Richiesta Header generata: '{full_request_str}'\n")

        # --- 3. Scrittura del File di Output ---
        print(f"Creazione del pacchetto di output '{args.output_file}'...")
        with open(args.output_file, 'wb') as f_out:
            # Scrive i tre pezzi in sequenza
            f_out.write(request_header_bytes)
            f_out.write(nonce_bytes)
            f_out.write(payload_bytes)

        print("\n  --- Successo! ---\n")
        print(f"File incapsulato creato: {args.output_file}")

    except Exception as e:
        print(f"Errore imprevisto durante l'elaborazione: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()