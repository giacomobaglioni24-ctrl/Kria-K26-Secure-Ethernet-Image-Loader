#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse

def write_hex_to_file(hex_string, output_file):
    try:
        byte_data = bytes.fromhex(hex_string)
        with open(output_file, "wb") as f:
            f.write(byte_data)
        print(f"Scrittura completata su '{output_file}' ({len(byte_data)} byte).")
    except ValueError:
        print("Errore: La stringa esadecimale contiene caratteri non validi.")
    except Exception as e:
        print(f"Errore durante la scrittura del file: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Scrive una stringa esadecimale in un file")
    parser.add_argument("-x", "--hex", required=True, help="Stringa esadecimale da scrivere")
    parser.add_argument("-o", "--output", required=True, help="Percorso del file di output")
    args = parser.parse_args()
    write_hex_to_file(args.hex, args.output)