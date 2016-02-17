#!/bin/bash

for j in internet2-1Gbps-10Gbps-tcp-buf-34250-70
      do
        python findavgfct-fair.py fifo fq fqcodel5ms codelboxFQLstf5ms-116800 $j 1 6
        python findavgdelay-fair.py fifo fq fqcodel5ms fqcodelLstf5ms codelboxFQLstf5ms-116800 $j 1 6
      done
