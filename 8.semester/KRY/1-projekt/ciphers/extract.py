import binascii

N = 100

lines = []

for j in range (0,N):
    with open("index.html?login=xlisci02." + str(j), "rb") as fp:
        cipher = fp.read()
        lines.append(binascii.hexlify(cipher))

for line in lines:
    print(line)
# with open("ciphers.txt", "wb") as fp1:
#     fp1.write(lines)