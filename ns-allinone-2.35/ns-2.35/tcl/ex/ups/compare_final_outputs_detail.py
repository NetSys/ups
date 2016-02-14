import sys

base_file = open(sys.argv[1]).readlines()
lstf_file = open(sys.argv[2]).readlines()
max_flow_id = 0

tmin = dict()
minTimes = open(sys.argv[3]).readlines()
for line in minTimes:
  words = line.split()
  tmin[(words[1],words[2])] = long(words[0])

out_file_compare = open(sys.argv[4], 'w')
out_file_ratios = open(sys.argv[5], 'w')

lstf_ts = dict()
for line in lstf_file:
  words = line.split()
  lstf_ts[(int(words[3]), int(words[4]))] = long(words[0])

count = 0
base_ts = dict()
for line in base_file:
  words = line.split()
  if(int(words[3]) > max_flow_id):
    max_flow_id = int(words[3])
  key = (int(words[3]), int(words[4]))
  base = tmin[(words[1], words[2])]
  pct_base = long(words[0]) - base
  if(pct_base < 0): #correcting 1ns rounding issues
    pct_base = 0
  try:
    pct_lstf = lstf_ts[key] - base
    if (pct_lstf < 0):
      pct_lstf = 0
    out_file_ratios.write(str(key[0]) + " " + str(key[1])  + " : " + " " + str(pct_lstf) + " " + str(pct_base) + " " + str(float(pct_lstf + 1)/float(pct_base + 1)) + " " + str(float(pct_lstf - pct_base)/float(pct_base + 1)) + "\n")
    if(pct_lstf > pct_base):
        count = count + 1
        out_file_compare.write(str(key[0]) + " " + str(key[1])  + " : " + str(base) + " " + str(pct_lstf) + " > " + str(pct_base) + " by " + str(long(pct_lstf) - long(pct_base)) + "\n")
		
  except:
      out_file_compare.write("Not in LSTF: " + str(key) + "\n")
out_file_compare.write(str(count) + " out of " + str(len(base_file)) + " ; Approx no. of flows = " + str(max_flow_id) + "\n")
