/* LSTF: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "lstf.h"
#include "tcp.h"
#include "rtp.h"

//#include "docsislink.h"

static class LstfClass : public TclClass {
  public:
    LstfClass() : TclClass("Queue/Lstf") {}
    TclObject* create(int, const char*const*) {
        return (new LstfQueue);
    }
} class_Lstf;

LstfQueue::LstfQueue() :  queueid_(-1), tchan_(0) 
{


    bind("curq_", &curq_);      // current queue size in bytes
    bind("debug_", &debug_);    // tcl settable ma
    bind("queueid_", &queueid_);
    bind("kTime_", &kTime_);
    bind("control_packets_", &control_packets_);
    bind("control_packets_time_", &control_packets_time_);
    bind("bandwidth_", &bandwidth_);
    char name[100];
    for (int i = 0; i < LSTF_NUM_QUEUES; i++) {
      sprintf(name, "queue_bounds_%d", i);
      bind(name, &q_bounds_[i+1]);
    }

    for(int i=0; i<=LSTF_NUM_QUEUES; i++) {
     bin_[i].q_ = new PacketQueue();
     bin_[i].index = i;
    }
   
    pq_ = bin_[1].q_; //does ns need this?
    reset();
}

void LstfQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;

    Queue::reset();
}



double LstfQueue::txtime(Packet* p) {
  return (8. * hdr_cmn::access(p)->size() / bandwidth_);
}

// Add a new packet to the queue. If the entire buffer space is full, drop highest slack packet
void LstfQueue::enque(Packet* pkt)
{
        hdr_ip *iph = hdr_ip::access(pkt);
        int seqNo = getSeqNo(pkt);
        long long int curSlack = iph->prio() + (long long int)(txtime(pkt) * kTime_);

  	// Drop a packet from the lowest priority queue if the buffer is full.
	if(curlen_ >= qlim_) {
	       for (int i = LSTF_NUM_QUEUES; i >= 1; i--) {
		   if (curSlack > q_bounds_[i]) {
			   drop(pkt);
			   return;
		   }
		   if ((bin_[i].q_)->length() == 0) continue;
                   Packet* tail_pkt = (bin_[i].q_)->tail();
                   (bin_[i].q_)->remove(tail_pkt);
                   curq_ -= HDR_CMN(tail_pkt)->size();
                   curlen_--; 
                   drop(tail_pkt);
	       }
        }
	curlen_++;
	curq_ += HDR_CMN(pkt)->size();
        
        //A hack to ensure same route construction when ECMP is enabled
        if((HDR_CMN(pkt)->size() >= 1460) || (!control_packets_)|| (Scheduler::instance().clock() >= double(control_packets_time_))) {
	
	    HDR_CMN(pkt)->ts_ = Scheduler::instance().clock();
	    for (int i = 1; i <= LSTF_NUM_QUEUES; i++) {
		    if (curSlack < q_bounds_[i]) (bin_[i].q_)->enque(pkt);
	    }

	    if(debug_)
        	printf("%lf: Lstf: QueueID %d: Enqueuing packet from flow with id %d, seqno = %d, size = %d and slack = %lld \n", Scheduler::instance().clock(), queueid_, iph->flowid(), seqNo, HDR_CMN(pkt)->size(), iph->prio()); 

        }
        //Enqueue control packets in higher priority control queue
        else {
            (bin_[0].q_)->enque(pkt);
	    if(debug_)
        	printf("%lf: Lstf: QueueID %d: Enqueuing packet from flow with id %d, seqno = %d, size = %d in control queue \n", Scheduler::instance().clock(), queueid_, iph->flowid(), seqNo, HDR_CMN(pkt)->size()); 

        }
}


Packet* LstfQueue::deque()
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
                printf("%lf: Lstf: QueueID %d: Dequing packet from flow with id %d, slack %lld, seqno = %d, size = %d from control queue\n", Scheduler::instance().clock(), queueid_, iph->flowid(), iph->prio(), seqNo, HDR_CMN(pkt)->size()); 
	      return pkt;
            } 
	    for (int i = 1; i <= LSTF_NUM_QUEUES; i--) {
                  if ((bin_[i].q_)->length() > 0) {
                      //if control queue empty, work on data queue
	              Packet *pkt;
	              pkt = (bin_[i].q_)->deque();
                      int seqNo = getSeqNo(pkt);
                      hdr_ip *iph = hdr_ip::access(pkt);
                      curlen_--;
                      curq_ -= HDR_CMN(pkt)->size();
	              
	
	              long long int wait_time = (Scheduler::instance().clock() * kTime_) - (long long int)(HDR_CMN(pkt)->ts_*kTime_);
	              long long int new_slack = iph->prio() - wait_time;
                      	
	              iph->prio() = new_slack;

	              if(debug_)
                      	printf("%lf: Lstf: QueueID %d: Dequing packet from flow with id %d, slack %lld, seqno = %d, size = %d \n", Scheduler::instance().clock(), queueid_, iph->flowid(), iph->prio(), seqNo, HDR_CMN(pkt)->size()); 

	              return pkt;
                  }
	    }
	}
       	return 0;
}



int LstfQueue::command(int argc, const char*const* argv)
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
                tcl.resultf("Lstf trace: can't attach %s for writing", id);
                return (TCL_ERROR);
            }
            return (TCL_OK);
        }
        // connect LSTF to the underlying queue
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
LstfQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "Lstf: unknown trace var %s\n", v->name());
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
