#!/bin/bash

if [ $# -lt 1 ];then 
    echo "Missing required parameter." 
    echo "Usage: ./tesh.sh number_count"
    return 1
elif ! [[ $1 =~ ^[0-9]+$ && $1 -ge 1 ]];then
    echo "Parameter number_count has to be a positive integer."
    return 1
else
    numbers=$1;
fi;

PROG=ots

#preklad
mpic++ --prefix /usr/local/share/OpenMPI -o $PROG $PROG.cpp

#vygenerovanie cisel
dd if=/dev/random bs=1 count=$numbers of=numbers >/dev/null 2>&1

#spustenie
mpirun --prefix /usr/local/share/OpenMPI -np $numbers $PROG

#mazanie
rm -f $PROG numbers
