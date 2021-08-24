import os

import sys


file = "vladiciphers/index.html?login=xmarci10.89"
file_size = os.path.getsize(file)

with open(file, "rb") as fp:
    cipher = fp.read(file_size)


znaky = []
for x in cipher:
    znaky.append(x)
print(znaky[0])
bytes(znaky)

# print(znaky)
with open("out.txt", "wb") as fp:
    fp.write(bytes(znaky))
