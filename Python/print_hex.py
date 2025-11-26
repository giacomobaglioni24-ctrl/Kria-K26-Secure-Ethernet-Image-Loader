#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse

def print_file_hex(file_path):
    try:
        with open(file_path, "rb") as f:
            data = f.read()
            hex_data = data.hex()
            for i in range(0, len(hex_data), 32):
                offset = i // 2
                chunk = hex_data[i:i+32]
                print(f"{offset:08x}: {chunk}")
    except FileNotFoundError:
        print(f"Errore: Il file '{file_path}' non è stato trovato.")
    except Exception as e:
        print(f"Errore durante la lettura del file: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Stampa contenuto esadecimale di un file")
    parser.add_argument("-i", "--input", required=True, help="Percorso del file da leggere")
    args = parser.parse_args()
    print_file_hex(args.input)
