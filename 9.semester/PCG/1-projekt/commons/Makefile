# Paralelní programování na GPU (PCG 2020)
# Projekt c. 1 (cuda)
# Login: xlogin00

# LIBS=-lhdf5 -I/usr/include/hdf5/serial
LIBS=-lhdf5 

.PHONY: all clean

all: gen

gen: gen.cpp h5Helper.cpp
	g++ gen.cpp h5Helper.cpp ${LIBS} -o gen

clean:
	rm -f *.o gen
