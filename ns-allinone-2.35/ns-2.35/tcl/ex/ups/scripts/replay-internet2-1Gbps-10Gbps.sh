#! /bin/bash
cd ..


#send a packet from every scource-dest on an empty network to get congestion-free minimum traversal time for a packet
./sim-udp-base.tcl  -simtime 10000 -gw DropTail -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-init.txt > replay-results/init.internet2-1Gbps-10Gbps

mv pcts.txt replay-results/init.internet2-1Gbps-10Gbps.pcts
mv fcts.txt replay-results/init.internet2-1Gbps-10Gbps.fcts

for util in 70
do

#Run replay for varying schedulers as follows:
#core = randomDequeue (packets to be dequeued are picked at random)
#core = DropTail (FIFO)
#core = Lifo
#core = sfqCoDel (the codel_target and maxbins parameters passed to script ensure that this is same as FQ)

for core in randomDequeue DropTail Lifo sfqCoDel 
do
	
        # get the original schedule
	./sim-udp-base.tcl  -simtime 1 -gw $core -maxq 100000000 -pktsize 1460 -codel_target 1000.0 -maxbins 1024 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt > replay-results/$core.internet2-1Gbps-10Gbps-$util
	
	mv pcts.txt replay-results/$core.internet2-1Gbps-10Gbps-$util.pcts
	mv fcts.txt replay-results/$core.internet2-1Gbps-10Gbps-$util.fcts
	mv queuesize.txt replay-results/$core.internet2-1Gbps-10Gbps-$util.queuesize
	echo "Original done"

        # get slacks
	python get_slacks.py replay-results/init.internet2-1Gbps-10Gbps.pcts replay-results/$core.internet2-1Gbps-10Gbps-$util.pcts replay-results/$core.internet2-1Gbps-10Gbps-$util.slacks

        wc -l < replay-results/$core.internet2-1Gbps-10Gbps-$util.slacks > slacks.txt
        cat replay-results/$core.internet2-1Gbps-10Gbps-$util.slacks >> slacks.txt
	 
        # do LSTF replay       
	./sim-udp-lstf.tcl  -simtime 1 -gw edgeReplay -maxq 100000000 -pktsize 1460 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt > replay-results/lstf-$core.internet2-1Gbps-10Gbps-$util
        mv pcts.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$util.pcts
        mv fcts.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$util.fcts
        mv queuesize.txt replay-results/lstf-$core.internet2-1Gbps-10Gbps-$util.queuesize
	echo "LSTF done"

        rm slacks.txt

        # compare original and replay pcts (Packet Completion Times)
	python compare_final_outputs_detail.py replay-results/$core.internet2-1Gbps-10Gbps-$util.pcts replay-results/lstf-$core.internet2-1Gbps-10Gbps-$util.pcts replay-results/init.internet2-1Gbps-10Gbps.pcts replay-results/$core.internet2-1Gbps-10Gbps-$util.compare replay-results/$core.internet2-1Gbps-10Gbps-$util.ratios 
      
        # find % of packets delayed 
        cd replay-results
        ./get-num-late-packets.sh

done
done
