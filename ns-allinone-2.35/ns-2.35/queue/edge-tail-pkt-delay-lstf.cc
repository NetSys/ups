/* Assigning slack for tail packet delay: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "edge-tail-pkt-delay-lstf.h"
#include "tcp.h"
#include "rtp.h"

#define kStoNs 1000000000
//#include "docsislink.h"

static class edgeTailPktDelayLstfClass : public TclClass {
  public:
    edgeTailPktDelayLstfClass() : TclClass("Queue/edgeTailPktDelayLstf") {}
    TclObject* create(int, const char*const*) {
        return (new edgeTailPktDelayLstfQueue);
    }
} class_edgeTailPktDelayLstf;

edgeTailPktDelayLstfQueue::edgeTailPktDelayLstfQueue() :  srcid_(-1), tchan_(0)
{


    bind("curq_", &curq_);      // current queue size in bytes
    bind("debug_", &debug_);    // tcl settable ma
    bind("srcid_", &srcid_);
    bind("initSlack_", &initSlack_);

    bin_.q_ = new PacketQueue();
    bin_.index = 0;

    pq_ = bin_.q_; //does ns need this?
    reset();
}

void edgeTailPktDelayLstfQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;
    Queue::reset();

}

// Add a new packet to the queue. If the entire buffer space is full, drop the packet.


void edgeTailPktDelayLstfQueue::enque(Packet* pkt)
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
        	printf("edgeTailPktDelayLstf: QueueID %d : %lf: Enqueuing packet from flow with id %d, seqno = %d, size = %d, slack = %lld \n", srcid_, Scheduler::instance().clock(), iph->flowid(), getSeqNo(pkt), HDR_CMN(pkt)->size(), iph->prio()); 

}

Packet* edgeTailPktDelayLstfQueue::deque()
{
	if(curlen_ > 0)
	{
		Packet *pkt;
		pkt = (bin_.q_)->deque();
        	hdr_ip *iph = hdr_ip::access(pkt);
                curlen_--;
                curq_ -= HDR_CMN(pkt)->size();

		iph->prio() = initSlack_ * kStoNs; //assigning same slack to all packets. Endhost delay has not been discounted.
                iph->init_slack() = iph->prio();

		if(debug_)
        		printf("edgeTailPktDelayLstf: Dequing packet from flow with id %d, slack %lld, seqno = %d, size = %d \n", iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size()); 


		return pkt;
	}
       	return 0;
}

int edgeTailPktDelayLstfQueue::command(int argc, const char*const* argv)
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
                tcl.resultf("edgeTailPktDelayLstf trace: can't attach %s for writing", id);
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
edgeTailPktDelayLstfQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "edgeTailPktDelayLstf: unknown trace var %s\n", v->name());
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
