cd ..

for buf in 34250
do
for i in 70
do

        # FQ Codel
	./sim-tcp-base.tcl -simtime 10 -gw sfqCoDel -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -codel_target 0.005 -maxbins 1024 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 > fqcodel-results/fqcodel5ms.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt fqcodel-results/fqcodel5ms.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

        # FIFO
	./sim-tcp-base.tcl -simtime 10 -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 > fqcodel-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt fqcodel-results/fifo.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

        # FQ
	./sim-tcp-base.tcl -simtime 10 -gw sfqCoDel -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -codel_target 1000 -maxbins 1024 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -pct_log 0 > fqcodel-results/fq.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt fqcodel-results/fq.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts

        # Edge CoDel with LSTF (FQ slack assignment with r_est = 100Mbps)
  for interSlack in 116800
  do
	./sim-tcp-lstf.tcl -simtime 10 -gw edgePrioFQLstf -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -codel_box 1 -codel_box_target 0.005 -topofolder scenarios/internet2-1Gbps-10Gbps -workloadfile workload-$i.txt -control_packets 0 -pct_log 0  -interSlack $interSlack > fqcodel-results/codelboxFQLstf5ms-$interSlack.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i
        mv fcts.txt fqcodel-results/codelboxFQLstf5ms-$interSlack.internet2-1Gbps-10Gbps-tcp-buf-$buf-$i.fcts
  done

done
done

#compute avg FCTS and RTTs
cd fqcodel-results/
./getaverage.sh
