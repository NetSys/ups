#! /bin/bash

cd ..

for i in 70 
do
       
	./sim-udp-base.tcl  -simtime 5 -gw DropTail -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt > tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$i
	mv pcts.txt tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$i.pcts
	mv fcts.txt tail-pkt-delays-results/fifo.internet2-1Gbps-10Gbps-$i.fcts

	./sim-udp-lstf.tcl  -simtime 5 -gw edgeTailPktDelayLstf -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt > tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$i
        mv pcts.txt tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$i.pcts
        mv fcts.txt tail-pkt-delays-results/tailPktDelayLstf.internet2-1Gbps-10Gbps-$i.fcts

        cd tail-pkt-delays-results
   
        python getFlowSizesTillSimTime-withTime.py ../scenarios/internet2-1Gbps-10Gbps/workload-$i.txt 5
        python findCompletionTimeCCdf-fair.py flowSizes-internet2.txt fifo.internet2-1Gbps-10Gbps-$i.pcts tailPktDelayLstf.internet2-1Gbps-10Gbps-$i.pcts fair-ccdf-pct-fifo.internet2-1Gbps-10Gbps-$i fair-ccdf-pct-tailPktDelayLstf.internet2-1Gbps-10Gbps-$i

done
