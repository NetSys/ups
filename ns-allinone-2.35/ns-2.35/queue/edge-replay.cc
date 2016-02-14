/* Slack initialization for LSTF replay: Author - Radhika Mittal, UC Berkeley, radhika@eecs.berkeley.edu */

#include <math.h>
#include <sys/types.h>
#include "config.h"
#include "template.h"
#include "random.h"
#include "edge-replay.h"
#include "tcp.h"
#include "rtp.h"

//#include "docsislink.h"

static class edgeReplayClass : public TclClass {
  public:
    edgeReplayClass() : TclClass("Queue/edgeReplay") {}
    TclObject* create(int, const char*const*) {
        return (new edgeReplayQueue);
    }
} class_edgeReplay;

edgeReplayQueue::edgeReplayQueue() :  srcid_(-1), tchan_(0)
{


    bind("curq_", &curq_);      // current queue size in bytes
    bind("debug_", &debug_);    // tcl settable ma
    bind("srcid_", &srcid_);
    bind("microsec_", &microsec_);

    bin_.q_ = new PacketQueue();
    bin_.index = 0;

    pq_ = bin_.q_; //does ns need this?
    reset();
}

void edgeReplayQueue::reset()
{
    curq_ = 0;
    curlen_ = 0;
    Queue::reset();

    
    if(srcid_ != -1) {
      FILE *fp = fopen("slacks.txt", "r");
      int num_packets;
      int err=fscanf(fp, "%d", &num_packets);
   
      int flowid, seqno, src, dest;
      long long int deadline;
      for (int i = 0; i < num_packets; i++) {
          err=fscanf(fp, "%d %d %d %d %lld", &src, &dest, &flowid, &seqno, &deadline);
          if(src == srcid_) {
            if(debug_) 
               printf("edgeReplay: QueueID %d: Adding entry (%d, %d, %d, %d, %lld) \n", srcid_, src, dest, flowid, seqno, deadline); 
            slack_map[std::make_pair(flowid, seqno)].deadline = deadline;
            slack_map[std::make_pair(flowid, seqno)].arrival_time = -1;
          }
      }
   }    
}

// Add a new packet to the queue. If the entire buffer space is full, drop the packet
void edgeReplayQueue::enque(Packet* pkt)
{
	// check for full buffer
        hdr_ip *iph = hdr_ip::access(pkt);

	if(slack_map.count(std::make_pair(iph->flowid(), getSeqNo(pkt))) == 0)
        {
           iph->prio() = 100000000000;
           slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].deadline = (Scheduler::instance().clock())*1000000000 + iph->prio();
	}
        else {
           iph->prio() = slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].deadline - (long long int)((Scheduler::instance().clock())*1000000000);
        }
        slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].arrival_time = (Scheduler::instance().clock())*1000000000;
        
        
	if(curlen_ >= qlim_) {
               drop(pkt);
               return;
        }


	curlen_++;
	curq_ += HDR_CMN(pkt)->size();
	(bin_.q_)->enque(pkt);
	if(debug_)
        	printf("edgeReplay: QueueID %d : Enqueuing packet from flow with id %d, seqno = %d, size = %d, slack = %lld \n", srcid_, iph->flowid(), getSeqNo(pkt), HDR_CMN(pkt)->size(), iph->prio()); 

}

Packet* edgeReplayQueue::deque()
{
	if(curlen_ > 0)
	{
		Packet *pkt;
		pkt = (bin_.q_)->deque();
        	hdr_ip *iph = hdr_ip::access(pkt);
                curlen_--;
                curq_ -= HDR_CMN(pkt)->size();
		long long int wait_time = (Scheduler::instance().clock() * 1000000000) - slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].arrival_time;
		long long int new_slack = iph->prio() - wait_time;
                
                if(microsec_) {
                  new_slack = (new_slack)/1000;
                }
		iph->prio() = new_slack;
                
		if(debug_)
        		printf("edgeReplay: Dequing packet from flow with id %d, slack %lld (%lld - %lld), seqno = %d, size = %d \n", iph->flowid(), iph->prio(), slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].deadline - slack_map[std::make_pair(iph->flowid(), getSeqNo(pkt))].arrival_time, wait_time, getSeqNo(pkt), HDR_CMN(pkt)->size()); 

		slack_map.erase(std::make_pair(iph->flowid(), getSeqNo(pkt)));
		return pkt;
	}
       	return 0;
}

int edgeReplayQueue::command(int argc, const char*const* argv)
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
                tcl.resultf("edgeReplay trace: can't attach %s for writing", id);
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
edgeReplayQueue::trace(TracedVar* v)
{
    const char *p;

    if ((p = strstr(v->name(), "curq")) == NULL) {
        fprintf(stderr, "edgeReplay: unknown trace var %s\n", v->name());
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
