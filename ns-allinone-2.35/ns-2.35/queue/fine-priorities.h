/*
 * finePrioritiesQueue - Simple label based priority queue
 */

#ifndef ns_finepriorities_h
#define ns_finepriorities_h

#include "queue.h"
#include <stdlib.h>
#include "agent.h"
#include "template.h"
#include "trace.h"
#include <map>
#include <utility>
#include "fstream"

struct bindesc {
        PacketQueue *q_;        // underlying FIFO queue
	int index;
} ;

class finePrioritiesQueue : public Queue {

  public:   
    finePrioritiesQueue();

  protected:
    void insertPacketinSortedQueue(Packet* pkt);
    void enque(Packet* pkt);
    int dropPacket(int pr);
    Packet* deque();
   
    bindesc bin_[2];

    int curlen_;	    // the total occupancy of all bins in packets
    int debug_;
    int queueid_;

    // NS-specific junk
    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records
    
    long int kTime_;
    int control_packets_;

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
