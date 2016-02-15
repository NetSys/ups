import sys

lines = open(sys.argv[1]).readlines()
simtime = float(sys.argv[2])


outlines = list()

count  = 0
for line in lines:
  words = line.split()
  if(float(words[0]) < simtime):
    outline = str(count) + " " + words[2] + " " + str(int(words[1])/1460) + " " + words[0] + "\n"
    outlines.append(outline)
    count = count + 1

f = open("flowSizes-internet2.txt", 'w')
for line in outlines:
  f.write(line)

