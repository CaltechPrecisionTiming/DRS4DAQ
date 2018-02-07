#!/bin/bash

beginrun="$1"
endrun="$2"

for i in `seq $beginrun $endrun`
do
    echo "running on run $i"
    ./Dat2RootV5 ~/LabData/Zhicai_Spring2018/data/$i.dat 2 
done
