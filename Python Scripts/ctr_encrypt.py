# DA LANCIARE SU BASH SE NECESSARIO: pip install pycryptodome

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
ctr_encrypt.py
Cifra un file usando AES-128-CTR (Nonce 64-bit, Counter 64-bit).
Utilizza la libreria standard 'pycryptodome'.

Uso:
python ctr_encrypt.py -i file_input.txt -o file_output.enc -k <key_hex> -n <nonce_hex>
"""

import sys
import argparse
from Crypto.Cipher import AES
from Crypto.Util import Counter

def encrypt_file(input_file, output_file, key_bytes, nonce_bytes):
    """Cifra il file usando AES-128-CTR."""
    
    try:
        # 1. Crea l'oggetto Counter per AES-CTR
        # prefix = Nonce (64 bit), nbits = dimensione Counter (64 bit)
        ctr = Counter.new(nbits=64, prefix=nonce_bytes)
        
        # 2. Crea il cifrario AES
        cipher = AES.new(key_bytes, AES.MODE_CTR, counter=ctr)
        
        # 3. Leggi, cifra e scrivi
        with open(input_file, 'rb') as f_in:
            with open(output_file, 'wb') as f_out:
                print(f"Lettura di '{input_file}'...")
                plaintext = f_in.read()
                
                print("Cifratura in corso...")
                ciphertext = cipher.encrypt(plaintext)
                
                f_out.write(ciphertext)
        
        print(f"\nSuccesso! File crittografato salvato in '{output_file}'.")
        print(f"  Totale byte letti: {len(plaintext)}")
        print(f"  Totale byte scritti: {len(ciphertext)}")

    except FileNotFoundError:
        print(f"Errore: Il file di input '{input_file}' non è stato trovato.")
        sys.exit(1)
    except Exception as e:
        print(f"Errore durante la cifratura: {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Encryptor AES-128-CTR")
    parser.add_argument("-i", "--input", dest="infile",
                        help="File di input da crittografare", required=True)
    parser.add_argument("-o", "--output", dest="outfile",
                        help="File di output dove salvare il cifrato", required=True)
    parser.add_argument("-k", "--key", dest="key_hex",
                        help="Chiave a 128 bit (32 caratteri hex)", required=True)
    parser.add_argument("-n", "--nonce", dest="nonce_hex",
                        help="Nonce a 64 bit (16 caratteri hex)", required=True)

    args = parser.parse_args()

    # --- Validazione Input ---
    if len(args.key_hex) != 32:
        print("Errore: La chiave deve essere di 32 caratteri esadecimali (128 bit).")
        sys.exit(1)
        
    if len(args.nonce_hex) != 16:
        print("Errore: Il Nonce deve essere di 16 caratteri esadecimali (64 bit).")
        sys.exit(1)
        
    try:
        key_bytes = bytes.fromhex(args.key_hex)
        nonce_bytes = bytes.fromhex(args.nonce_hex)
    except ValueError:
        print("Errore: Chiave o Nonce contengono caratteri non validi.")
        sys.exit(1)

    encrypt_file(args.infile, args.outfile, key_bytes, nonce_bytes)

if __name__ == "__main__":
    main()