# =============================================================

import os
import base64
import binascii

N = 100
file = "ciphers/index.html?login=xlisci02.0"
file_size = os.path.getsize(file)

def lusti(guess):
    # print("-"*80)
    # jsem = list("Instrumentovany odehravanejsi predkrajovany fistron dotresen ZP osadnik avantgarda ")
    ascii_key = []
    current_guess = list(guess)

    #nacitanie zo subora
    with open(file, "rb") as fp:
        cipher = fp.read(len(current_guess))
    # hadanie kluca
    for i in range(len(current_guess)):
        ascii_key.append(ord(current_guess[i])^(cipher[i]))
    # print("key:", ascii_key)
    sum = 0
    words = []
    for j in range (0,N):
        with open("ciphers/index.html?login=xlisci02." + str(j), "rb") as fp:
            cipher = fp.read(len(current_guess))
        #prevod na plaintext pomocou hadaneho kluca
        # print("plaintext:")
        word = []
        for i in range(len(cipher)):
            char_dec = (ascii_key[i]^cipher[i])
            if i==0 and (char_dec <65 or char_dec > 90):
                break
            if (chr(char_dec) not in chars):
                break
            word.append(chr(char_dec))
        if len(word) == len(current_guess):
            # print(''.join(word))
            words.append(''.join(word))
            sum += 1
    if sum == N:
        # print("key:", ascii_key, k)
        # print("----------------------------Success", words)
        return (0)
    # ascii_key.clear()
    return (1)

def get_key(plaintext):
    ascii_key = []
    with open(file, "rb") as fp:
        cipher = fp.read(len(plaintext))

    for i in range(len(plaintext)):
        ascii_key.append(ord(plaintext[i])^(cipher[i]))
    return ascii_key


# guess = "Bosakuv vycouvani rozplynuti sestikilometrovy vrapencuv Tuvalanka zareagovat hotelieruv panuv zverejnovat tahajici karny Demeteruv mulatka demonstrovat vysadkarstvi neznaboh vymamit Chocholuv pentametr Valvoda sberna Cisteves objezdni hevlinsky Rikonin vymoz pramen smekani atropin kolchoznikuv podfakturovat Radova kurz navesti"
# guess = "Pilotuv ischiaticky pristehovalectvi doprogramovat roztlapat bacil prehravany pektin odmastovadlo Toncin osmitydenni stepovaci listovni podvalovany navagonovat strojar dvacetitisicovy zvazenejsi rozmahani hormon chroupat prelobbovat vestecky rujnovat vydouvaci Podyji Filipi preochotny Adamcikova doregulovat pecivarna Straseci "
guess = ""
# guess = "Panikarstvi zprogresivnovani stujme holdingovy odeznivat lidovecky objatejsi povelovy prehrazovat zapleveleny presmerovany skomiravy vlcatinsky vulgo vlivnejsi Lisalova zplynovavat Bushuv obehnanejsi bioklimatictejsi vidle Ryvolova idealnejsi blufovany starovlastenka Cvrcovice kompilace pasovec rozpoznavac prostejsi pretvorit"
chars = ['.', ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M']
while(len(guess)<file_size):
    flag = 0
    for c in chars:
        if not (lusti(guess+c)):
            guess = guess+c
            print(guess)
            flag=1
            break
    if not flag:
        break

# print(len(guess))
import binascii
import codecs
key = get_key(guess)

print("--------------------------")
print(binascii.hexlify(bytes(key)))

with open("xlisci02.txt", "wb") as fp:
    fp.write(bytes(key))

# with open("xlogin00.txt", "wb") as file:
#     a = base64.b64encode(key)
#     b = base64.b64decode(a)
#     file.write(b)