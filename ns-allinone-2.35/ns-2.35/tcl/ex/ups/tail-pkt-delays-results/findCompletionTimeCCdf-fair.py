import sys
import numpy

lines0 = open(sys.argv[1]).readlines()
lines1 = open(sys.argv[2]).readlines()
lines2 = open(sys.argv[3]).readlines()


flowids = list()
times1 = list()
times2 = list()

for line in lines0:
  words = line.split()
  if((float(words[3]) > 0.5) and (float(words[3]) < 2)):
    flowids.append(int(words[0]))

for line in lines1:
  words = line.split()
  if(((long(words[6]) - long(words[0])) > 500000000)):  
    if(int(words[3]) in flowids):
      times1.append(float(words[0])/1000000000)
    
for line in lines2:
  words = line.split()
  if(((long(words[6]) - long(words[0])) > 500000000)):  
    if(int(words[3]) in flowids):
      times2.append(float(words[0])/1000000000)


outfile1_ccdf = open(sys.argv[4] + "-ccdf", "w")
outfile2_ccdf = open(sys.argv[5] + "-ccdf", "w")
outfile3 = open(sys.argv[4] + "-mean", "w")
outfile4 = open(sys.argv[5] + "-mean", "w")

outfile1_ccdf.write("0.0001 " + str(numpy.percentile(times1, (100-0.0001))) + "\n")
outfile1_ccdf.write("0.001 " + str(numpy.percentile(times1, (100-0.001))) + "\n")
outfile1_ccdf.write("0.01 " + str(numpy.percentile(times1, (100-0.01))) + "\n")
outfile1_ccdf.write("0.1 " + str(numpy.percentile(times1, (100-0.1))) + "\n")
for i in xrange(1,100):
  outfile1_ccdf.write(str(i) + " " + str(numpy.percentile(times1, int(100 - i))) + "\n")


outfile2_ccdf.write("0.0001 " + str(numpy.percentile(times2, (100-0.0001))) + "\n")
outfile2_ccdf.write("0.001 " + str(numpy.percentile(times2, (100-0.001))) + "\n")
outfile2_ccdf.write("0.01 " + str(numpy.percentile(times2, (100-0.01))) + "\n")
outfile2_ccdf.write("0.1 " + str(numpy.percentile(times2, (100-0.1))) + "\n")
for i in xrange(1,100):
  outfile2_ccdf.write(str(i) + " " + str(numpy.percentile(times2, int(100 - i))) + "\n")

outfile3.write(str(sum(times1)/len(times1)) + "\n")
outfile4.write(str(sum(times2)/len(times2)) + "\n")


