import os
import sys
import numpy



def findavg(input_file):
  fcts = dict()
  lines = input_file.readlines()
  for line in lines:
      words  = line.split()
      starttime = (float(words[6]))/1000000000
      if((starttime >= float(sys.argv[-2])) and (starttime <= float(sys.argv[-1]))):
                size = (int(words[4])) * 1460
                fcts[int(words[3])] = (size, (float(words[0])/1000000000))
  return fcts  


def findfctbysize(fcts):
  fcts_new = dict()
  for key in fcts.keys():
    if(fcts[key][0] not in fcts_new.keys()):
      fcts_new[fcts[key][0]] = list()
    fcts_new[fcts[key][0]].append(fcts[key][1])
  return fcts_new



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

  fcts_list = list()
  for input_file in input_files:
    fcts_list.append(findavg(input_file))

  fcts_list_new = list()
  for fcts in fcts_list:
    fcts_new = findfctbysize(fcts)
    fcts_list_new.append(fcts_new)

  for inp in xrange(0 , len(input_files)):

    fcts = fcts_list_new[inp]

    corename = sys.argv[inp + 1] + "." + sys.argv[-3]

    fctsbulk = list()
    fctsweighted = list()
    sizes = list()
  
    count  = 0
    f1 = open("averages/"+corename, 'w')
    f2 = open("averages/bulk-"+corename, 'w')
    f3 = open("averages/weighted-"+corename, 'w')
    f4 = open("averages/bucket-"+corename, 'w')
 

    bucket = dict()
    bucket["1pkt"] = list()
    bucket["2KB-20KB"] = list()
    bucket["50KB-500KB"] = list()
    bucket["1MB-2MB"] = list()
    bucket["3MB"] = list()
    f = dict()

    for bucketkey in bucket.keys():
      f[bucketkey] = open("averages/" + bucketkey + "-" + corename, 'w')

    for key in sorted(fcts.keys()):
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

      for x in fcts[key]:
          fctsbulk.append(x)
          # bucketing flows
          fctsweighted.append(key*x)
          sizes.append(key)
          bucket[bucketkey].append(x)
          f[bucketkey].write(str(x) + "\n")

      avg = sum(fcts[key])/len(fcts[key])
      med = numpy.percentile(fcts[key], int(50))
      perc99 = numpy.percentile(fcts[key], int(99))
      perc95 = numpy.percentile(fcts[key], int(95))
      perc1 = numpy.percentile(fcts[key], int(1))
      perc10 = numpy.percentile(fcts[key], int(10))
      
      length = len(fcts[key])
      
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
       
    avg = sum(fctsbulk)/len(fctsbulk)
    med = numpy.percentile(fctsbulk, int(50))
    perc99 = numpy.percentile(fctsbulk, int(99))
    perc1 = numpy.percentile(fctsbulk, int(1))
    perc10 = numpy.percentile(fctsbulk, int(10))
    length = len(fctsbulk)
    f2.write("avg_over_flows" + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(length) + "\n")

    avg = sum(fctsweighted)/sum(sizes)
    med = numpy.percentile(fctsweighted, int(50))
    perc99 = numpy.percentile(fctsweighted, int(99))
    perc1 = numpy.percentile(fctsweighted, int(1))
    perc10 = numpy.percentile(fctsweighted, int(10))
    length = len(fctsweighted)
    f3.write("avg_over_bytes" + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(length) + "\n")
  
    f1.close()
    f2.close()
    f3.close()
