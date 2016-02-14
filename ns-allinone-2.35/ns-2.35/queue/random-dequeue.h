/* Random Dequeuer: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#ifndef ns_randomdequeue_h
#define ns_randomdequeue_h

#include "queue.h"
#include <stdlib.h>
#include "agent.h"
#include "template.h"
#include "trace.h"
#include "fstream"

#define MAXBINS 2
#define DEBUG 1


struct bindesc {
        PacketQueue *q_;        // underlying FIFO queue
	int index;
} ;

class randomDequeueQueue : public Queue {
  public:   
    randomDequeueQueue();


  protected:
    void enque(Packet* pkt);
    int dropPacket(int pr);
    Packet* deque();
   
    bindesc bin_[MAXBINS];
    

    int curlen_;	    // the total occupancy of all bins in packets
    int maxbins_;	    // for tcl override of MAXBINS (can only make smaller)
    int debug_;
    int maxprio_;
    int logid_;
    int wblog_;
    int routelog_;

    // NS-specific junk
    int command(int argc, const char*const* argv);
    void reset();
    void trace(TracedVar*); // routine to write trace records

    Tcl_Channel tchan_;     // place to write trace records
    TracedInt curq_;        // current qlen in bytes seen by arrivals

};

#endif
