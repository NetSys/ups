 #!/bin/bash

for i in 70
do
  for core in randomDequeue DropTail Lifo sfqCoDel
  do
    python findLatePackets.py $core.internet2-1Gbps-10Gbps-$i.compare 11680 > late-packets-$core-internet2-1Gbps-10Gbps-$i
  done
done







