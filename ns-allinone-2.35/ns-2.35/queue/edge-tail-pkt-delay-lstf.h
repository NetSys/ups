/* Assigning slack for tail packet delay*/

#ifndef ns_edgetailpktdelaylstf_h
#define ns_edgetailpktdelaylstf_h

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



class edgeTailPktDelayLstfQueue : public Queue {
  public:   
    edgeTailPktDelayLstfQueue();

  protected:
    // Stuff specific to the CoDel algorithm
    void enque(Packet* pkt);
    int dropPacket(int pr);
    Packet* deque();
   
    bindesc bin_;

    int curlen_;	    // the total occupancy of all bins in packets
    int debug_;
    int srcid_;
    double initSlack_;

    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
