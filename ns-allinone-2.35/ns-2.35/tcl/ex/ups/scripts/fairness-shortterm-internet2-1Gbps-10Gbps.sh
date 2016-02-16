#! /bin/bash

cd ..

for buf in 34250
do
for util in 70 
do

        # Run FQ
	./sim-tcp-base.tcl -simtime 10 -gw sfqCoDel -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -codel_target 1000 -maxbins 1024 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -pct_log 0 > fairness-results/shortterm/fq.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt fairness-results/shortterm/fq.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts

          
        # Run FIFO
	./sim-tcp-base.tcl -simtime 10 -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -pct_log 0 > fairness-results/shortterm/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
        mv fcts.txt fairness-results/shortterm/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts

       # Run FQ LSTF with varying r_est (10Mbps, 100Mbps, 200Mbps, 500Mbps)   
       for interSlack in 1168000 116800 58400 23360
       do
  	  ./sim-tcp-lstf.tcl -simtime 10 -gw edgeFQLstf -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$util.txt -control_packets 0 -pct_log 0  -interSlack $interSlack > fairness-results/shortterm/FQLstf-$interSlack.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util
          mv fcts.txt fairness-results/shortterm/FQLstf-$interSlack.internet2-1Gbps-10Gbps-tcp-buf-$buf-$util.fcts
       done
     
done
done

#compute average FCTs
cd fairness-results/shortterm/
./getaverage.sh
