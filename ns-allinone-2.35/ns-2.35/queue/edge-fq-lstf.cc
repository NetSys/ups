/*
 * edgeFQLstf - Set slacks for Fair Queuing; Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu 
 */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "edge-fq-lstf.h"
#include "tcp.h"
#include "rtp.h"

#define kStoNs 1000000000

//#include "docsislink.h"

static class edgeFQLstfClass : public TclClass {
  public:
    edgeFQLstfClass() : TclClass("Queue/edgeFQLstf") {}
    TclObject* create(int, const char*const*) {
        return (new edgeFQLstfQueue);
    }
} class_edgeFQLstf;

edgeFQLstfQueue::edgeFQLstfQueue() :  srcid_(-1), interSlack_(0), tchan_(0)
{


    bind("curq_", &curq_);      // current queue size in bytes
    bind("debug_", &debug_);    // tcl settable ma
    bind("srcid_", &srcid_);
    bind("interSlack_", &interSlack_);

    bin_.q_ = new PacketQueue();
    bin_.index = 0;

    pq_ = bin_.q_; //does ns need this?
    reset();
}

void edgeFQLstfQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;
    Queue::reset();

}

// Add a new packet to the queue. If the entire buffer space is full, drop the packet
void edgeFQLstfQueue::enque(Packet* pkt)
{
	// check for full buffer
        hdr_ip *iph = hdr_ip::access(pkt);

	if(curlen_ >= qlim_) {
               drop(pkt);
               return;
        }

	curlen_++;
	curq_ += HDR_CMN(pkt)->size();
	(bin_.q_)->enque(pkt);
	if(debug_)
        	printf("edgeFQLstf: QueueID %d : Enqueuing packet from flow with id %d, seqno = %d, size = %d, slack = %lld \n", srcid_, iph->flowid(), getSeqNo(pkt), HDR_CMN(pkt)->size(), iph->prio()); 

}

Packet* edgeFQLstfQueue::deque()
{
	if(curlen_ > 0)
	{
		Packet *pkt;
		pkt = (bin_.q_)->deque();
        	hdr_ip *iph = hdr_ip::access(pkt);
                curlen_--;
                curq_ -= HDR_CMN(pkt)->size();

                //if no packets of this flow have been seen
                if(!per_flow_map.count(iph->flowid())) {
                    iph->prio() = 0;
                }
                else {
                     //if the packet has already spent all it's slack by arriving late (well-behaved)
                     if(((long long int)(Scheduler::instance().clock() * kStoNs) - per_flow_map[iph->flowid()].arrival_time) 
                             > per_flow_map[iph->flowid()].orig_slack + (long long int)interSlack_) { 
                        iph->prio() = 0;
                     }  else { //if there are bursty arrivals 
                        iph->prio() = per_flow_map[iph->flowid()].orig_slack +  (long long int)interSlack_ - 
                           ((long long int)(Scheduler::instance().clock() * kStoNs) - per_flow_map[iph->flowid()].arrival_time);
                     }
                }
                per_flow_map[iph->flowid()].orig_slack = iph->prio();
                per_flow_map[iph->flowid()].arrival_time = (long long int)(Scheduler::instance().clock() * kStoNs);
                iph->init_slack() = iph->prio();
		if(debug_)
        		printf("edgeFQLstf: Dequing packet from flow with id %d, slack %lld, seqno = %d, size = %d \n", iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size()); 

		return pkt;
	}
       	return 0;
}

int edgeFQLstfQueue::command(int argc, const char*const* argv)
{
    Tcl& tcl = Tcl::instance();

    if (argc == 2) {
        if (strcmp(argv[1], "reset") == 0) {
            reset();
            return (TCL_OK);
        } 
    } else if (argc == 3) {
        // attach a file for variable tracing
        if (strcmp(argv[1], "attach") == 0) {
            int mode;
            const char* id = argv[2];
            tchan_ = Tcl_GetChannel(tcl.interp(), (char*)id, &mode);
            if (tchan_ == 0) {
                tcl.resultf("edgeFQLstf trace: can't attach %s for writing", id);
                return (TCL_ERROR);
            }
            return (TCL_OK);
        }
        // connect queue to the underlying queue
        if (!strcmp(argv[1], "packetqueue-attach")) {
//            delete q_;
//            if (!(q_ = (PacketQueue*) TclObject::lookup(argv[2])))
		printf("error in command\n");
                return (TCL_ERROR);
//            else {
//                pq_ = q_;
//                return (TCL_OK);
//            }
        }
    }
    return (Queue::command(argc, argv));
}

// Routine called by TracedVar facility when variables change values.
// Note that the tracing of each var must be enabled in tcl to work.
void
edgeFQLstfQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "edgeFQLstf: unknown trace var %s\n", v->name());
        return;
    }
    if (tchan_) {
        char wrk[500];
        double t = Scheduler::instance().clock();
        if(*p == 'c') {
            sprintf(wrk, "c %g %d", t, int(*((TracedInt*) v)));
        }
        int n = strlen(wrk);
        wrk[n] = '\n'; 
        wrk[n+1] = 0;
        (void)Tcl_Write(tchan_, wrk, n+1);
    }
}
