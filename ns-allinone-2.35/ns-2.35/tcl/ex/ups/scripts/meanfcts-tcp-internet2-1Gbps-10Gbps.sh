#! /bin/bash
cd ..
simtime=10

for buf in 3425
do
for util in 70 
do

           # run baseline fifo
	./sim-tcp-base.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -pct_log 0 > meanfcts-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt meanfcts-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts

           # run LSTF with SJF priority assignment
	./sim-tcp-lstf.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -pct_log 0 -sjfLstf 1 -control_packets 0 > meanfcts-results/sjfLstf.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt meanfcts-results/sjfLstf.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts

         # run SJF with priorities
	./sim-tcp-priorities.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -sjfPrio 1 -pct_log 0 > meanfcts-results/sjfPrio.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt meanfcts-results/sjfPrio.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts

         # run SRPT
	./sim-tcp-srpt.tcl -simtime $simtime -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000  -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -pct_log 0 > meanfcts-results/srpt.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt meanfcts-results/srpt.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts
 
        # find mean FCTs (which get stored in meanfcts-results/averages folder)
        cd meanfcts-results
        ./getaverage.sh

done
done
