#!/bin/bash

for j in internet2-1Gbps-10Gbps-tcp-buf-3425-70 
      do
        python findavg-fair.py fifo srpt sjfPrio sjfLstf $j 0.5 2
      done
