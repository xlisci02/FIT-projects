import os
import binascii
plaintext="Bosakuv vycouvani rozplynuti sestikilometrovy vrapencuv Tuvalanka zareagovat hotelieruv panuv zverejnovat tahajici karny Demeteruv mulatka demonstrovat vysadkarstvi neznaboh vymamit Chocholuv pentametr Valvoda sberna Cisteves objezdni hevlinsky Rikonin vymoz pramen smekani atropin kolchoznikuv podfakturovat Radova kurz navesti c"
file_size = os.path.getsize("xlisci02.txt")

with open("xlisci02.txt", "rb") as fp:
    key = fp.read(file_size)


cipher = []
for i in range(len(plaintext)):
    cipher.append(chr(
       ord(plaintext[i]) ^ 
        key[i])
    )
print(cipher)


print(''.join(cipher))
with open("output.txt", "w") as fp:
    for c in cipher:
        fp.write(hex(ord(c))[2:])