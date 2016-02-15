#! /bin/bash
# Compare Bufferbloat+FQ with CoDel+FCFS
# Workload: one long running TCP that is interested in
# either: throughput/delay or throughput.

#./remove-vestiges.sh
cd ..

./sim-udp-base.tcl  -simtime 10000 -gw DropTail -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-init.txt > replay-results/init.internet2-1Gbps-10Gbps

mv pcts.txt replay-results/init.internet2-1Gbps-10Gbps.pcts
mv fcts.txt replay-results/init.internet2-1Gbps-10Gbps.fcts

for i in 70
do

for core in randomDequeue DropTail Lifo sfqCoDel 
do
	

	./sim-udp-base.tcl  -simtime 1 -gw $core -maxq 100000000 -pktsize 1460 -codel_target 1000.0 -maxbins 1024 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt > replay-results/$core.internet2-1Gbps-10Gbps-$i
	
	mv pcts.txt replay-results/$core.internet2-1Gbps-10Gbps-$i.pcts
	mv fcts.txt replay-results/$core.internet2-1Gbps-10Gbps-$i.fcts
	mv queuesize.txt replay-results/$core.internet2-1Gbps-10Gbps-$i.queuesize
	echo "Original done"

	python get_slacks.py replay-results/init.internet2-1Gbps-10Gbps.pcts replay-results/$core.internet2-1Gbps-10Gbps-$i.pcts replay-results/$core.internet2-1Gbps-10Gbps-$i.slacks

        wc -l < replay-results/$core.internet2-1Gbps-10Gbps-$i.slacks > slacks.txt
        cat replay-results/$core.internet2-1Gbps-10Gbps-$i.slacks >> slacks.txt
	        
	./sim-udp-lstf.tcl  -simtime 1 -gw edgeReplay -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt > replay-results/lstf-$core.internet2-1Gbps-10Gbps-$i
        mv pcts.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$i.pcts
        mv fcts.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$i.fcts
        mv queuesize.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$i.queuesize
	echo "LSTF done"

        rm slacks.txt

	python compare_final_outputs_detail.py replay-results/$core.internet2-1Gbps-10Gbps-$i.pcts replay-results/lstf-$core.internet2-1Gbps-10Gbps-$i.pcts replay-results/init.internet2-1Gbps-10Gbps.pcts replay-results/$core.internet2-1Gbps-10Gbps-$i.compare replay-results/$core.internet2-1Gbps-10Gbps-$i.ratios 

done
done
