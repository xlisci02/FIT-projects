#!/bin/bash

if [ $# -ne 1 ];then
    if [ $# -lt 1 ];then
        echo "Missing required parameter."
    else
        echo "Too many parameters."
    echo "Usage: ./tesh.sh list_of_altitudes"
    return 1
    fi;
else
    if ! [[ $1 =~ ^((0|[1-9][0-9]*),)*(0|[1-9][0-9]*)$ ]];then
        echo "Parameter list_of_altitudes has to be a list of natural numbers."
        return 1
    fi;
    alts=(${1//,/ })
    # pocet hodnot
    numvals=${#alts[@]}
    # pocet procesorov
    numprocs=`python3 -c "import math; MERLIN_LIMIT=25;\
        opt_numprocs = min(len(
            [$numvals - p*math.log(p,2) for p in range(1,$numvals+1) if $numvals - p*math.log(p,2) >= 0]),\
            MERLIN_LIMIT);\
        print(int(math.pow(2,math.floor(math.log(opt_numprocs,2)))))"`
fi;

PROG=vid

#preklad
mpic++ --prefix /usr/local/share/OpenMPI -o $PROG $PROG.cpp

#spustenie
mpirun --prefix /usr/local/share/OpenMPI -np $numprocs $PROG $1
