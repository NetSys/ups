/*
 * edgeReplayQueue - Simple label based priority queue
 */

#ifndef ns_edgepriobulk_h
#define ns_edgepriobulk_h

#include "queue.h"
#include <stdlib.h>
#include "agent.h"
#include "template.h"
#include "trace.h"
#include <map>
#include <utility>

#define MAXFLOWS 6000
#define MAXSEQ 3000
#define DEBUG 1


struct bindesc {
        PacketQueue *q_;        // underlying FIFO queue
	int index;
} ;

struct Slack {
  long long int deadline;
  long long int arrival_time;
} ;

typedef std::map<std::pair<int, int>, Slack> SlackMap;

class edgeReplayQueue : public Queue {
  public:   
    edgeReplayQueue();


  protected:
    void enque(Packet* pkt);
    int dropPacket(int pr);
    Packet* deque();
   
    bindesc bin_;

    int curlen_;	    // the total occupancy of all bins in packets
    int debug_;
    SlackMap slack_map;
    int srcid_;
    int microsec_;

    // NS-specific junk
    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
