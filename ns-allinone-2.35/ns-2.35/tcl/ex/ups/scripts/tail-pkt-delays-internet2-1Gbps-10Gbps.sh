#! /bin/bash

cd ..

for util in 70 
do
       
        # run FIFO
	./sim-udp-base.tcl  -simtime 5 -gw DropTail -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt > tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$util
	mv pcts.txt tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$util.pcts
	mv fcts.txt tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$util.fcts

        # run LSTF (same as FIFO+)
	./sim-udp-lstf.tcl  -simtime 5 -gw edgeTailPktDelayLstf -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt > tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$util
        mv pcts.txt tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$util.pcts
        mv fcts.txt tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$util.fcts

        cd tail-pkt-delays-results
   
        # compute complementary CDF for packet delays (packet completion times)
        python getFlowSizesTillSimTime-withTime.py ../scenarios/internet2-1Gbps-10Gbps/workload-$util.txt 5
        python findCompletionTimeCCdf-fair.py flowSizes-internet2.txt fifo.internet2-1Gbps-10Gbps-$util.pcts tailPktDelayLstf.internet2-1Gbps-10Gbps-$util.pcts fair-ccdf-pct-fifo.internet2-1Gbps-10Gbps-$util fair-ccdf-pct-tailPktDelayLstf.internet2-1Gbps-10Gbps-$util

done
