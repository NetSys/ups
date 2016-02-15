import sys

lines = open(sys.argv[1]).readlines()

def findfairness(tpt):
  fairness_num = sum(tpt)*sum(tpt)
  fairness_den = 0 
  for x in tpt:
    fairness_den = fairness_den + (x*x)
  fairness_den = fairness_den*len(tpt)
  fairness = fairness_num/fairness_den
  return fairness

pktsPerMs = dict()
for line in lines:
  words = line.split()
  time_ms = int(float(words[5])*1000)
  if(time_ms not in pktsPerMs.keys()):
    pktsPerMs[time_ms] = dict()
  key2 = int(words[3])
  if(key2 not in pktsPerMs[time_ms].keys()):
    pktsPerMs[time_ms][key2] = 0
  pktsPerMs[time_ms][key2] = pktsPerMs[time_ms][key2] + 1


fairness = dict()
for key in pktsPerMs.keys():
  tpt = list()
  for i in xrange(0,90):
    if(i not in pktsPerMs[key].keys()):
      tpt.append(0)
    else:
      tpt.append(float(pktsPerMs[key][i]*1460*8)/1000)
  fairness[key] = findfairness(tpt)

for key in sorted(pktsPerMs.keys()):
  print str(key) + " " + str(fairness[key])
