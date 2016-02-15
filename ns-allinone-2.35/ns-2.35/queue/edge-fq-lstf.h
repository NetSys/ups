/*
 * edgeFQLstf - Set slacks for Fair Queuing
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

struct bindesc {
        PacketQueue *q_;        // underlying FIFO queue
	int index;
} ;

struct Slack {
  long long int orig_slack;
  long long int arrival_time;
} ;

typedef std::map<int, Slack> PerFlowMap;

class edgeFQLstfQueue : public Queue {
  public:   
    edgeFQLstfQueue();

  protected:
    // Stuff specific to the CoDel algorithm
    void enque(Packet* pkt);
    int dropPacket(int pr);
    Packet* deque();
   
    bindesc bin_;

    int curlen_;	    // the total occupancy of all bins in packets
    int debug_;
    PerFlowMap per_flow_map;
    int srcid_;
    int interSlack_;
    double refresh_threshold_;

    // NS-specific junk
    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
