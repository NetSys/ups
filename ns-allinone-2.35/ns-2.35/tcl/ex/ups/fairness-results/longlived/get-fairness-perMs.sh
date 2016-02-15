 #!/bin/bash

for core in fifo fq fqLstf-11680 fqLstf-23360 fqLstf-116800 fqLstf-233600 fqLstf-1168000
do
  python findFairness-per1ms.py $core.internet2-10Gbps-10Gbps-tcp-buf-34250-fq-5ms.pcts > fairness-$core-internet2-10Gbps-10Gbps-tcp-buf-34250-fq-5ms
done
