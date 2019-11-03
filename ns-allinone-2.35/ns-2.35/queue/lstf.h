/*
 * LstfQueue 
 */

#ifndef ns_lstf_h
#define ns_lstf_h

#include "queue.h"
#include <stdlib.h>
#include "agent.h"
#include "template.h"
#include "trace.h"
#include <map>
#include <utility>
#include "fstream"

#define DEBUG 1
#define LSTF_NUM_QUEUES 8

struct bindesc {
        PacketQueue *q_;        // underlying FIFO queue
	int index;
} ;

class LstfQueue : public Queue {

  public:   
    LstfQueue();

  protected:
    void enque(Packet* pkt);
    int dropPacket(int pr);
    double txtime(Packet* p);
    Packet* deque();
   
    bindesc bin_[1 + LSTF_NUM_QUEUES];
    long int q_bounds_[1 + LSTF_NUM_QUEUES];

    int curlen_;	    // the total occupancy of all bins in packets
    int debug_;
    int queueid_;

    // NS-specific junk
    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records
    
    long int kTime_;
    int control_packets_;
    int control_packets_time_;
    double bandwidth_;

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
