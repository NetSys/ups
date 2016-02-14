import sys

def get_t_min(init_pcts, t_min):

  for line in init_pcts:
    words = line.split()
    t_min[(words[1],words[2])] = long(words[0]) 


def get_deadlines(sched_pcts, t_min, deadlines):
  for line in sched_pcts:
    words = line.split()
    deadline = long(words[6]) - t_min[(words[1],words[2])]
    deadlines[(words[1],words[2], words[3], words[4])] = deadline


if __name__ == "__main__":
  init_pcts = open(sys.argv[1]).readlines()
  sched_pcts = open(sys.argv[2]).readlines()
  outfile = open(sys.argv[3], "w")

  t_min = dict()
  get_t_min(init_pcts, t_min)

  deadlines = dict()
  get_deadlines(sched_pcts, t_min, deadlines)
  
  for key in deadlines.keys():
    outfile.write(key[0] + " " + key[1] + " " + key[2] + " " + key[3] + " " + str(deadlines[key]) + "\n")
    
