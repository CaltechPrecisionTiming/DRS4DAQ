#! /bin/bash 

caen-01 --dataset-name $1  --run-number $2 --max-triggers $3 --run --sample-rate 5 --num-samples 1024 --trigger-edge falling --trigger-delay 0 --digitize-trigger 
