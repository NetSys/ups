#! /bin/bash
# Compare Bufferbloat+FQ with CoDel+FCFS
# Workload: one long running TCP that is interested in
# either: throughput/delay or throughput.

#./remove-vestiges.sh
cd ..
simtime=10

for buf in 3425
do
for i in 70 
do

	./sim-tcp-base.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 > meanfcts-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt meanfcts-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

	./sim-tcp-lstf.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 -sjfLstf 1 -control_packets 0 > meanfcts-results/sjfLstf.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt meanfcts-results/sjfLstf.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

	./sim-tcp-priorities.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -sjfPrio 1 -pct_log 0 > meanfcts-results/sjfPrio.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt meanfcts-results/sjfPrio.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

	./sim-tcp-srpt.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 > meanfcts-results/srpt.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt meanfcts-results/srpt.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

done
done
