#! /bin/bash 

caen-02 --dataset-name $1  --run-number $2 --max-triggers $3 --run --sample-rate 5 --num-samples 1024 --trigger-edge rising --trigger-delay 0 --digitize-trigger
