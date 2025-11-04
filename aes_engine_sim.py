#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
aes_engine_sim.py
Simulatore didattico per l'intero processo di cifratura AES-128 (10 round).

Uso:
python aes_engine_sim.py -k 000102030405060708090a0b0c0d0e0f -p 00112233445566778899aabbccddeeff
"""

import sys
import argparse

# --- Costanti e Funzioni del Key Expansion (prese dallo script 1) ---

S_BOX = (
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
)

RCON = [
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
]

def bytes_to_words(key_bytes):
    return [int.from_bytes(key_bytes[i:i+4], 'big') for i in range(0, 16, 4)]

def words_to_bytes(words):
    b = bytearray()
    for w in words:
        b.extend(w.to_bytes(4, 'big'))
    return bytes(b)

def rot_word(word):
    return ((word << 8) & 0xFFFFFFFF) | (word >> 24)

def sub_word(word):
    b0 = S_BOX[(word >> 24) & 0xFF]
    b1 = S_BOX[(word >> 16) & 0xFF]
    b2 = S_BOX[(word >>  8) & 0xFF]
    b3 = S_BOX[(word >>  0) & 0xFF]
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3

def generate_key_schedule(key_bytes):
    """Genera lo schedule di 11 chiavi (come liste di 16 byte)."""
    key_schedule_words = bytes_to_words(key_bytes)
    key_schedule = [words_to_bytes(key_schedule_words)]
    for i in range(1, 11):
        w0, w1, w2, w3 = key_schedule_words
        temp = w3
        temp_rot = rot_word(temp)
        temp_sub = sub_word(temp_rot)
        rcon_val = RCON[i] << 24
        temp_xor = temp_sub ^ rcon_val
        w4 = w0 ^ temp_xor
        w5 = w1 ^ w4
        w6 = w2 ^ w5
        w7 = w3 ^ w6
        key_schedule_words = [w4, w5, w6, w7]
        key_schedule.append(words_to_bytes(key_schedule_words))
    return key_schedule

# --- Funzioni AES Engine ---

def bytes_to_state(b):
    """Converte 16 byte in una matrice 4x4 (colonna-per-colonna)."""
    return [
        [b[0], b[4], b[8],  b[12]],
        [b[1], b[5], b[9],  b[13]],
        [b[2], b[6], b[10], b[14]],
        [b[3], b[7], b[11], b[15]],
    ]

def state_to_bytes(m):
    """Converte una matrice 4x4 in 16 byte (colonna-per-colonna)."""
    return bytes([m[r][c] for c in range(4) for r in range(4)])

def print_state(state, title=""):
    """Stampa lo stato (matrice) in formato esadecimale."""
    print(f"\n  {title}:")
    for r in range(4):
        print("    " + " ".join(f"{state[r][c]:02x}" for c in range(4)))

def add_round_key(state, key_bytes):
    """Esegue AddRoundKey (XOR)."""
    key_state = bytes_to_state(key_bytes)
    for r in range(4):
        for c in range(4):
            state[r][c] ^= key_state[r][c]
    return state

def sub_bytes(state):
    """Esegue SubBytes (S-Box lookup)."""
    for r in range(4):
        for c in range(4):
            state[r][c] = S_BOX[state[r][c]]
    return state

def shift_rows(state):
    """Esegue ShiftRows."""
    # Riga 0: nessun shift
    # Riga 1: shift di 1 a sx
    state[1][0], state[1][1], state[1][2], state[1][3] = \
    state[1][1], state[1][2], state[1][3], state[1][0]
    # Riga 2: shift di 2 a sx
    state[2][0], state[2][1], state[2][2], state[2][3] = \
    state[2][2], state[2][3], state[2][0], state[2][1]
    # Riga 3: shift di 3 a sx
    state[3][0], state[3][1], state[3][2], state[3][3] = \
    state[3][3], state[3][0], state[3][1], state[3][2]
    return state

# --- Funzioni MixColumns (xtime) ---

def xtime(b):
    """Logica x2 (moltiplicazione per 02 nel GF(2^8))."""
    if b & 0x80:
        return ((b << 1) ^ 0x1B) & 0xFF
    else:
        return (b << 1) & 0xFF

def x3(b):
    """Logica x3 (moltiplicazione per 03 nel GF(2^8))."""
    return xtime(b) ^ b

def mix_columns(state):
    """Esegue MixColumns."""
    for c in range(4):
        # Estrai la colonna
        s0 = state[0][c]
        s1 = state[1][c]
        s2 = state[2][c]
        s3 = state[3][c]
        
        # Calcola i nuovi valori (come da formule)
        s0_new = xtime(s0) ^ x3(s1)   ^ s2        ^ s3
        s1_new = s0        ^ xtime(s1) ^ x3(s2)   ^ s3
        s2_new = s0        ^ s1        ^ xtime(s2) ^ x3(s3)
        s3_new = x3(s0)   ^ s1        ^ s2        ^ xtime(s3)
        
        # Reinserisci la colonna (assicurati che siano byte)
        state[0][c] = s0_new & 0xFF
        state[1][c] = s1_new & 0xFF
        state[2][c] = s2_new & 0xFF
        state[3][c] = s3_new & 0xFF
    return state

# --- Main ---
def main():
    parser = argparse.ArgumentParser(description="Simulatore AES-128 Engine")
    parser.add_argument("-k", "--key", dest="key_hex",
                        help="Chiave a 128 bit (32 caratteri hex)",
                        required=True)
    parser.add_argument("-p", "--plaintext", dest="pt_hex",
                        help="Plaintext a 128 bit (32 caratteri hex)",
                        required=True)
    
    args = parser.parse_args()
    
    if len(args.key_hex) != 32 or len(args.pt_hex) != 32:
        print("Errore: Chiave e Plaintext devono essere di 32 caratteri esadecimali.")
        sys.exit(1)
        
    try:
        key_bytes = bytes.fromhex(args.key_hex)
        pt_bytes = bytes.fromhex(args.pt_hex)
    except ValueError:
        print("Errore: La chiave o il plaintext contengono caratteri non validi.")
        sys.exit(1)

    print(f"--- SIMULAZIONE AES-128 ENGINE ---")
    print(f"Key:       {key_bytes.hex()}")
    print(f"Plaintext: {pt_bytes.hex()}")
    
    # 1. Key Expansion
    print("\n--- 1. Key Expansion ---")
    key_schedule = generate_key_schedule(key_bytes)
    
    # 2. Inizializzazione
    print("\n--- 2. Cifratura (Round per Round) ---")
    state = bytes_to_state(pt_bytes)
    print_state(state, "Stato Iniziale (Input)")
    
    # --- Round 0 ---
    print("\n=== ROUND 0 ===")
    state = add_round_key(state, key_schedule[0])
    print_state(state, "AddRoundKey (K0)")

    # --- Round 1-9 ---
    for r in range(1, 10):
        print(f"\n=== ROUND {r} ===")
        state = sub_bytes(state)
        print_state(state, "SubBytes")
        state = shift_rows(state)
        print_state(state, "ShiftRows")
        state = mix_columns(state)
        print_state(state, "MixColumns")
        state = add_round_key(state, key_schedule[r])
        print_state(state, f"AddRoundKey (K{r})")

    # --- Round 10 (Finale) ---
    print(f"\n=== ROUND 10 (Finale) ===")
    state = sub_bytes(state)
    print_state(state, "SubBytes")
    state = shift_rows(state)
    print_state(state, "ShiftRows")
    # (No MixColumns)
    state = add_round_key(state, key_schedule[10])
    print_state(state, "AddRoundKey (K10)")
    
    # --- Risultato ---
    ciphertext = state_to_bytes(state)
    print("\n--- RISULTATO FINALE (Ciphertext) ---")
    print(ciphertext.hex())

if __name__ == "__main__":
    main()