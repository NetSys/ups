#!/bin/bash

for j in internet2-1Gbps-10Gbps-tcp-buf-34250-70 
do
  python findavg-fair.py fifo fq $j 1 6
for interSlack in 1168000 116800 58400 23360
do
  python findavg-fair.py FQLstf-$interSlack $j 1 6
done
done

