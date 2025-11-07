key_expansion_sim.py - Per simulare l'espansione di una chiave

cd C:\Users\stage_gbaglioni\Desktop\Tesi\Python Scripts

python key_expansion_sim.py -k 000102030405060708090a0b0c0d0e0f





aes_engine_sim.py - Per simulare l'encryptazione di un blocco

cd C:\Users\stage_gbaglioni\Desktop\Tesi\Python Scripts

python aes_engine_sim.py -p 00112233445566770000000000000002 -k 00112233445566778899aabbccddeeff





ctr_encrypt.py - Per criptare un file (Il nonce non viene incluso)

cd C:\Users\stage_gbaglioni\Desktop\Tesi\Python Scripts

python .\ctr_encrypt.py -i plaintext.txt -o chipertext.txt -k 00112233445566778899aabbccddeeff -n 0011223344556677





print_hex.py - Per leggere il contenuto in Hex

python print_hex.py -i plaintext.txt

python print_hex.py -i chipertext.txt





write_hex.py - Per scrivere il contenuto in Hex 

python write_hex.py -x "000102030405060708090A0B0C0D0E0F" -o plaintext.txt

