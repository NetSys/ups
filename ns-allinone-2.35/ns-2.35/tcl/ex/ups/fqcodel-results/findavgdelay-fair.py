import os
import sys
import numpy



def findavg(input_file):
  delays = dict()
  lines = input_file.readlines()
  for line in lines:
      words  = line.split()
      starttime = (float(words[6]))/1000000000
      if((starttime >= float(sys.argv[-2])) and (starttime <= float(sys.argv[-1]))):
                size = (int(words[4])) * 1460
                tpt = (float(size) * 8.0) / (float(words[0])/1000)
                delay = float(words[7]) / 1000
                delays[int(words[3])] = (size, delay)
  return delays  


def finddelaysbysize(delays):
  delays_new = dict()
  for key in delays.keys():
    if(delays[key][0] not in delays_new.keys()):
      delays_new[delays[key][0]] = list()
    delays_new[delays[key][0]].append(delays[key][1])
  return delays_new



if __name__ == "__main__":
  if len(sys.argv) < 2:
    print "Usage:", sys.argv[0], "<input_file1> <input_file2> <input_file3> <input_file4> <outputfile1 format>" 
    sys.exit(1)

  input_files = list()
  try:
    for arg in sys.argv[1:-3]:
      input_files.append(open("1." + arg + "." + sys.argv[-3] + ".fcts"))
      print "Opened 1." + arg + "." + sys.argv[-3] + ".fcts"
  except:
    print "Error opening 1." + arg + "." + sys.argv[-3] + ".fcts"
    sys.exit(1)

  delays_list = list()
  for input_file in input_files:
    delays_list.append(findavg(input_file))

  delays_list_new = list()
  for delays in delays_list:
    delays_new = finddelaysbysize(delays)
    delays_list_new.append(delays_new)

  for x in xrange(0 , len(input_files)):

    delays = delays_list_new[x]

    corename = sys.argv[x + 1] + "." + sys.argv[-3]

    delaysbulk = list()
    delaysweighted = list()
    sizes = list()
  
    count  = 0
    f1 = open("avgdelays/"+corename, 'w')
    f2 = open("avgdelays/bulk-"+corename, 'w')
    f3 = open("avgdelays/weighted-"+corename, 'w')
    f4 = open("avgdelays/bucket-"+corename, 'w')
 

    bucket = dict()
    bucket["1pkt"] = list()
    bucket["2KB-20KB"] = list()
    bucket["50KB-500KB"] = list()
    bucket["1MB-2MB"] = list()
    bucket["3MB"] = list()
    f = dict()

    for bucketkey in bucket.keys():
      f[bucketkey] = open("avgdelays/" + bucketkey + "-" + corename, 'w')


    for key in sorted(delays.keys()):

      if(key == 1460):
        bucketkey = "1pkt"
      if((key >= 2920) and (key <= 10220)):
        bucketkey = "2KB-20KB"
      if((key >= 58400) and (key <= 389820)):
        bucketkey = "50KB-500KB"
      if(key == 1733020):
        bucketkey  = "1MB-2MB"
      if(key == 3076220):
        bucketkey = "3MB"

      
      for x in delays[key]:
          delaysbulk.append(x)
          delaysweighted.append(key*x)
          sizes.append(key)
          bucket[bucketkey].append(x)
          f[bucketkey].write(str(x) + "\n")

      avg = sum(delays[key])/len(delays[key])
      med = numpy.percentile(delays[key], int(50))
      perc99 = numpy.percentile(delays[key], int(99))
      perc95 = numpy.percentile(delays[key], int(95))
      perc1 = numpy.percentile(delays[key], int(1))
      perc10 = numpy.percentile(delays[key], int(10))
      
      length = len(delays[key])
      
      print str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length)
      
      f1.write(str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length) + "\n")
      
    for key in ["1pkt", "2KB-20KB", "50KB-500KB", "1MB-2MB", "3MB"]:    
      f[key].close()
      avg = sum(bucket[key])/len(bucket[key])
      med = numpy.percentile(bucket[key], int(50))
      perc99 = numpy.percentile(bucket[key], int(99))
      perc95 = numpy.percentile(bucket[key], int(95))
      perc1 = numpy.percentile(bucket[key], int(1))
      perc10 = numpy.percentile(bucket[key], int(10))
      
      length = len(bucket[key])
      
      print str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length)
      
      f4.write(str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length) + "\n")
 
    avg = sum(delaysbulk)/len(delaysbulk)
    med = numpy.percentile(delaysbulk, int(50))
    perc99 = numpy.percentile(delaysbulk, int(99))
    perc1 = numpy.percentile(delaysbulk, int(1))
    perc10 = numpy.percentile(delaysbulk, int(10))
    length = len(delaysbulk)
    f2.write("avg_over_flows" + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(length) + "\n")

    avg = sum(delaysweighted)/sum(sizes)
    med = numpy.percentile(delaysweighted, int(50))
    perc99 = numpy.percentile(delaysweighted, int(99))
    perc1 = numpy.percentile(delaysweighted, int(1))
    perc10 = numpy.percentile(delaysweighted, int(10))
    length = len(delaysweighted)
    f3.write("avg_over_bytes" + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(length) + "\n")
  
    f1.close()
    f2.close()
    f3.close()
