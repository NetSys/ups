/* Fine-grained priority queue: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "fine-priorities.h"
#include "tcp.h"
#include "rtp.h"

//#include "docsislink.h"
static class finePrioritiesClass : public TclClass {
  public:
    finePrioritiesClass() : TclClass("Queue/finePriorities") {}
    TclObject* create(int, const char*const*) {
        return (new finePrioritiesQueue);
    }
} class_finePriorities;

finePrioritiesQueue::finePrioritiesQueue() :  queueid_(-1), tchan_(0) 
{


    bind("curq_", &curq_);      // current queue size in bytes
    bind("debug_", &debug_);    // tcl settable ma
    bind("queueid_", &queueid_);
    bind("kTime_", &kTime_);
    bind("control_packets_", &control_packets_);

    for(int i=0; i<2; i++) {
     bin_[i].q_ = new PacketQueue();
     bin_[i].index = i;
    }
    pq_ = bin_[1].q_; //does ns need this?
    reset();
}

void finePrioritiesQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;

    Queue::reset();
}

// Add a new packet to the queue. If the entire buffer space is full, drop from
// the lowest priority queue.
                
void finePrioritiesQueue::insertPacketinSortedQueue(Packet* pkt) {

  hdr_ip *iph = hdr_ip::access(pkt);
  long long int curPrio = iph->prio();

  for(Packet *pp= 0, *p= (bin_[1].q_)->head(); p; pp= p, p= p->next_) {
    int tempSeqNo = getSeqNo(p);
    hdr_ip *tempIph = hdr_ip::access(p);
    long long int prio = tempIph->prio();
    if(prio > curPrio) {
      if (p == (bin_[1].q_)->head()) {
        (bin_[1].q_)->enqueHead(pkt);
      }
      else {
        (bin_[1].q_)->enqueAfterPkt(pkt, pp);
      }
      return;
    }
  }

  (bin_[1].q_)->enque(pkt);

}




void finePrioritiesQueue::enque(Packet* pkt)
{
        hdr_ip *iph = hdr_ip::access(pkt);
        int seqNo = getSeqNo(pkt);

  	// check for full buffer
	if(curlen_ >= qlim_) {
               if((bin_[1].q_)->tail() == 0) {
                 drop(pkt);
                 return;
               } 
               Packet* tail_pkt = (bin_[1].q_)->tail();
               hdr_ip *tail_pkt_iph = hdr_ip::access(tail_pkt);
               int tail_pkt_seqNo = getSeqNo(tail_pkt);
               long long int tail_pkt_prio = tail_pkt_iph->prio();
               if(iph->prio() > tail_pkt_prio) {
                 drop(pkt);
                 return;
               }
               if (debug_)
                  printf("%lf: finePriorities: QueueID %d: Dropped packet with id %d and sequence %d\n", 
                               Scheduler::instance().clock(), queueid_, tail_pkt_iph->flowid(), tail_pkt_seqNo);
               (bin_[1].q_)->remove(tail_pkt);
               curq_ -= HDR_CMN(tail_pkt)->size();
               curlen_--; 
               drop(tail_pkt);
        }
	curlen_++;
	curq_ += HDR_CMN(pkt)->size();
        
        //If control packets are to be seperated
        if((HDR_CMN(pkt)->size() >= 1460) || (!control_packets_)) {
	
	    HDR_CMN(pkt)->ts_ = Scheduler::instance().clock();
	    insertPacketinSortedQueue(pkt);

	    if(debug_)
        	printf("%lf: finePriorities: QueueID %d: Enqueuing packet from flow with id %d, seqno = %d, size = %d and prio = %lld \n", Scheduler::instance().clock(), queueid_, iph->flowid(), seqNo, HDR_CMN(pkt)->size(), iph->prio()); 

        }
        //Enqueue control packets in higher priority control queue
        else {
            (bin_[0].q_)->enque(pkt);
	    if(debug_)
        	printf("%lf: finePriorities: QueueID %d: Enqueuing packet from flow with id %d, seqno = %d, size = %d in control queue \n", Scheduler::instance().clock(), queueid_, iph->flowid(), seqNo, HDR_CMN(pkt)->size()); 

        }
}

Packet* finePrioritiesQueue::deque()
{
	if(curlen_ > 0)
	{
              //first check control queue
                if((bin_[0].q_)->length() > 0)
	        {
		    Packet *pkt;
		    pkt = (bin_[0].q_)->deque();
        	    int seqNo = getSeqNo(pkt);
        	    hdr_ip *iph = hdr_ip::access(pkt);
                    curlen_--;
                    curq_ -= HDR_CMN(pkt)->size();
		    if(debug_)
        	  	printf("%lf: finePriorities: QueueID %d: Dequing packet from flow with id %d, prio %lld, seqno = %d, size = %d from control queue\n", Scheduler::instance().clock(), queueid_, iph->flowid(), iph->prio(), seqNo, HDR_CMN(pkt)->size()); 
		    return pkt;
                 } 
               
                //if control queue empty, work on data queue
		Packet *pkt;
		pkt = (bin_[1].q_)->deque();
        	int seqNo = getSeqNo(pkt);
        	hdr_ip *iph = hdr_ip::access(pkt);
                curlen_--;
                curq_ -= HDR_CMN(pkt)->size();

		if(debug_)
        		printf("%lf: finePriorities: QueueID %d: Dequing packet from flow with id %d, prio %lld, seqno = %d, size = %d \n", Scheduler::instance().clock(), queueid_, iph->flowid(), iph->prio(), seqNo, HDR_CMN(pkt)->size()); 
		return pkt;
	}
       	return 0;
}

int finePrioritiesQueue::command(int argc, const char*const* argv)
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
                tcl.resultf("finePriorities trace: can't attach %s for writing", id);
                return (TCL_ERROR);
            }
            return (TCL_OK);
        }
        // connect priority queue to the underlying queue
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
finePrioritiesQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "finePriorities: unknown trace var %s\n", v->name());
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
