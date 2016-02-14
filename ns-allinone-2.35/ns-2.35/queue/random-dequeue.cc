/* Random Dequeuer: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "random-dequeue.h"
#include "tcp.h"
#include "rtp.h"

//#include "docsislink.h"

static class randomDequeueClass : public TclClass {
  public:
    randomDequeueClass() : TclClass("Queue/randomDequeue") {}
    TclObject* create(int, const char*const*) {
        return (new randomDequeueQueue);
    }
} class_randomDequeue;

randomDequeueQueue::randomDequeueQueue() :  maxbins_(MAXBINS), tchan_(0)
{
   bind("curq_", &curq_);      // current queue size in bytes
   bind("maxbins_", &maxbins_);    // tcl settable max number of bins
   bind("debug_", &debug_);    // tcl settable max number of bins
					// if zero, rounds by packets
   
   if (maxbins_ > MAXBINS)  {
        printf("randomDequeue: maxbins_ of %d exceeds upper bound of %d", maxbins_, MAXBINS);
	exit(0);
    }
  
    for(int i=0; i<2; i++) {
     bin_[i].q_ = new PacketQueue();
     bin_[i].index = i;
    }

    pq_ = bin_[1].q_; //does ns need this?
    reset();
}

void randomDequeueQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;
    Queue::reset();
}

// Add a new packet to the queue. If the entire buffer space is full, drop the packet


void randomDequeueQueue::enque(Packet* pkt)
{
	// check for full buffer
        hdr_ip *iph = hdr_ip::access(pkt);

	if(curlen_ >= qlim_) {
          drop(pkt);
          return;
        }
	curlen_++;
	curq_ += HDR_CMN(pkt)->size();

        //hack to ensure same route computation when ECMP is enabled
        if (HDR_CMN(pkt)->size() >= 1460) {
	  (bin_[1].q_)->enque(pkt);
	  if(debug_)
        	printf("randomDequeue: Enqueuing packet from flow with id %d, slack %d, seqno = %d, size = %d in queue with curlen = %d\n", iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size(), curlen_); 
       }
       else {
	  (bin_[0].q_)->enque(pkt);
	  if(debug_)
        	printf("randomDequeue: Enqueuing packet from flow with id %d, slack %d, seqno = %d, size = %d in queue with curlen = %d in control queue \n", iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size(), curlen_); 
       }
}

Packet* randomDequeueQueue::deque()
{

       if(curlen_ > 0) {

           if((bin_[0].q_)->length() > 0) {
		    Packet *pkt;
		    pkt = (bin_[0].q_)->deque();
        	    hdr_ip *iph = hdr_ip::access(pkt);
                    curlen_--;
                    curq_ -= HDR_CMN(pkt)->size();
		    if(debug_)
        	  	printf("%lf: randomDequeue: Dequing packet from flow with , slack %lld, seqno = %d, size = %d from control queue\n", Scheduler::instance().clock(), iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size()); 
		    return pkt;
          } 



          int pktNum = rand() % curlen_;
	  Packet *pkt = (bin_[1].q_)->lookup(pktNum);
	  (bin_[1].q_)->remove(pkt);
          hdr_ip *iph = hdr_ip::access(pkt);
          curlen_--;
          curq_ -= HDR_CMN(pkt)->size();

	  if(debug_)
            printf("randomDequeue: Dequing packet from flow with id %d, slack %d, seqno = %d, size = %d from queue with curlen = %d\n", iph->flowid(), iph->prio(), getSeqNo(pkt), HDR_CMN(pkt)->size(), curlen_); 
	  return pkt;
       }
       

       if(debug_)
       		printf("randomDequeue: Nothing to dequeue\n");
       return 0;
}

int randomDequeueQueue::command(int argc, const char*const* argv)
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
                tcl.resultf("randomDequeue trace: can't attach %s for writing", id);
                return (TCL_ERROR);
            }
            return (TCL_OK);
        }
        // connect slack queue to the underlying queue
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
randomDequeueQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "randomDequeue: unknown trace var %s\n", v->name());
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
