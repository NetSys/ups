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
      input_files.append(open(arg + "." + sys.argv[-3] + ".fcts"))
      print "Opened " + arg + "." + sys.argv[-3] + ".fcts"
  except:
    print "Error opening " + arg + "." + sys.argv[-3] + ".fcts"
    sys.exit(1)

  fcts_list = list()
  for input_file in input_files:
    fcts_list.append(findavg(input_file))

  fcts_list_new = list()
  for fcts in fcts_list:
    fcts_new = findfctbysize(fcts)
    fcts_list_new.append(fcts_new)

  for x in xrange(0 , len(input_files)):

    fcts = fcts_list_new[x]

    corename = sys.argv[x + 1] + "." + sys.argv[-3]

    fctsbulk = list()
    fctsweighted = list()
    sizes = list()
  
    count  = 0
    f1 = open("averages/"+corename, 'w')
    f2 = open("averages/bulk-"+corename, 'w')
    f3 = open("averages/weighted-"+corename, 'w')
 

    for key in sorted(fcts.keys()):
      
      for x in fcts[key]:
          fctsbulk.append(x)
          fctsweighted.append(key*x)
          sizes.append(key)

      avg = sum(fcts[key])/len(fcts[key])
      med = numpy.percentile(fcts[key], int(50))
      perc99 = numpy.percentile(fcts[key], int(99))
      perc95 = numpy.percentile(fcts[key], int(95))
      perc1 = numpy.percentile(fcts[key], int(1))
      perc10 = numpy.percentile(fcts[key], int(10))
      
      length = len(fcts[key])
      
      print str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length)
      
      f1.write(str(key) + "\t" + str(avg) + "\t" + str(med) + "\t" + str(perc99) + "\t" + str(perc1) + "\t" + str(perc10) + "\t" + str(perc95) + "\t" + str(length) + "\n")
      
   
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
