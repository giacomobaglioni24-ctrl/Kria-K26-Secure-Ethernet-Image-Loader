# DA LANCIARE SU BASH SE NECESSARIO: pip install pycryptodome

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
encrypt.py
Cifra un file usando AES-128-CTR (Nonce 64-bit, Counter 64-bit).
Genera un nonce casuale e imposta il contatore iniziale a 1.
Utilizza la libreria standard 'pycryptodome'.

Uso:
python encrypt.py -i file_input.txt -o file_output.enc -k <key_hex>
"""

import sys
import argparse
from Crypto.Cipher import AES
from Crypto.Util import Counter
from Crypto.Random import get_random_bytes

def encrypt_file(input_file, output_file, key_bytes, nonce_bytes):
    """Cifra il file usando AES-128-CTR."""
    
    # Definiamo il valore iniziale del contatore
    INITIAL_COUNTER_VALUE = 1
    
    try:
        # 1. Crea l'oggetto Counter per AES-CTR
        # prefix = Nonce (64 bit)
        # nbits = dimensione Counter (64 bit)
        # initial_value = 1 (come da richiesta)
        ctr = Counter.new(
            nbits=64, 
            prefix=nonce_bytes, 
            initial_value=INITIAL_COUNTER_VALUE
        )
        
        # 2. Crea il cifrario AES
        cipher = AES.new(key_bytes, AES.MODE_CTR, counter=ctr)
        
        # 3. Leggi, cifra e scrivi
        with open(input_file, 'rb') as f_in:
            with open(output_file, 'wb') as f_out:
                print(f"\nLettura di '{input_file}'...")
                plaintext = f_in.read()
                
                print("\nCifratura in corso...")
                ciphertext = cipher.encrypt(plaintext)
                
                f_out.write(ciphertext)
        
        # --- RIEPILOGO ---
        # Costruisci il primo blocco contatore (16 byte) usato per la cifratura
        # (8 byte di nonce + 8 byte di contatore = 1)
        counter_start_bytes = INITIAL_COUNTER_VALUE.to_bytes(8, 'big')
        first_counter_block_hex = nonce_bytes.hex() + counter_start_bytes.hex()
        nonce_random_vector_hex = nonce_bytes.hex() + get_random_bytes(8).hex()

        print(f"\nSuccesso! File crittografato salvato in '{output_file}'.")
        print(f"\n  Nonce (Prefisso) (8 byte hex):")
        print(f"    {nonce_bytes.hex()}")
        print(f"\n  Primo Blocco Contatore (16 byte hex):")
        print(f"    {first_counter_block_hex}")
        print(f"\n  Totale byte letti: {len(plaintext)}")
        print(f"  Totale byte scritti: {len(ciphertext)}")
        print(f"\n  Vettore Random per passare il Nonce (16 byte hex):")
        print(f"    {nonce_random_vector_hex}")

    except FileNotFoundError:
        print(f"Errore: Il file di input '{input_file}' non è stato trovato.")
        sys.exit(1)
    except Exception as e:
        print(f"Errore durante la cifratura: {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Encryptor AES-128-CTR con Nonce Casuale e Counter=1")
    parser.add_argument("-i", "--input", dest="infile",
                        help="File di input da crittografare", required=True)
    parser.add_argument("-o", "--output", dest="outfile",
                        help="File di output dove salvare il cifrato", required=True)
    parser.add_argument("-k", "--key", dest="key_hex",
                        help="Chiave a 128 bit (32 caratteri hex)", required=True)

    args = parser.parse_args()

    # --- Validazione Input ---
    if len(args.key_hex) != 32:
        print("Errore: La chiave deve essere di 32 caratteri esadecimali (128 bit).")
        sys.exit(1)
        
    try:
        key_bytes = bytes.fromhex(args.key_hex)
    except ValueError:
        print("Errore: La chiave contiene caratteri non validi.")
        sys.exit(1)

    # --- Generazione Nonce ---
    print("\nGenerazione di un nonce casuale a 64-bit (8 byte)...")
    # Genera 8 byte per il prefisso (Nonce)
    nonce_bytes = get_random_bytes(8) 
    
    print(f"\n  Nonce: {nonce_bytes.hex()}")
    print(f"\n  Counter Iniziale: 1\n")
    print("-" * 30)

    # --- Esecuzione Cifratura ---
    encrypt_file(args.infile, args.outfile, key_bytes, nonce_bytes)

if __name__ == "__main__":
    main()
