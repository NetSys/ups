#! /bin/bash

cd ..

for buf in 34250 
do
     # run FQ
    ./sim-tcp-base.tcl -simtime 0.02 -gw sfqCoDel -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -codel_target 1000.0 -maxbins 1024 -topofolder scenarios/internet2-10Gbps-10Gbps -workloadfile workload-fq-5ms.txt -us_delay 1 > fairness-results/longlived/fq.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms
    mv pcts.txt fairness-results/longlived/fq.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.pcts
    mv fcts.txt fairness-results/longlived/fq.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.fcts

    # run FIFO 
    ./sim-tcp-base.tcl -simtime 0.02 -gw DropTail -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -topofolder scenarios/internet2-10Gbps-10Gbps -workloadfile workload-fq-5ms.txt -us_delay 1 > fairness-results/longlived/fifo.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms
    mv pcts.txt fairness-results/longlived/fifo.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.pcts
    mv fcts.txt fairness-results/longlived/fifo.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.fcts

    # Run FQ-LSTF with varying $r_est$ values
    # r_est = 1Gbps 500Mbps 100Mbps 50Mbps 10Mbps
    for interSlack in 11680 23360 116800 233600 1168000
    do 
	./sim-tcp-lstf.tcl -simtime 0.02 -gw edgeFQLstf -tcp TCP/Newreno -sink TCPSink/Sack1 -maxq $buf -pktsize 1460 -rcvwin 10000000 -topofolder scenarios/internet2-10Gbps-10Gbps -workloadfile workload-fq-5ms.txt -interSlack $interSlack -us_delay 1 -control_packets 0 > fairness-results/longlived/fqLstf-$interSlack.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms
        mv pcts.txt fairness-results/longlived/fqLstf-$interSlack.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.pcts
        mv fcts.txt fairness-results/longlived/fqLstf-$interSlack.internet2-10Gbps-10Gbps-tcp-buf-$buf-fq-5ms.fcts
    done
done

#compute fairness per ms
cd fairness-results/longlived/
./get-fairness-perMs.sh
