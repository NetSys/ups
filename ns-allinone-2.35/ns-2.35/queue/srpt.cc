/* SRPT: from Mohammad Alizadeh's code */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/queue/drop-tail.cc,v 1.17 2004/10/28 23:35:37 haldar Exp $ (LBL)";
#endif

#include "srpt.h"

static class SRPTClass : public TclClass {
 public:
	SRPTClass() : TclClass("Queue/SRPT") {}
	TclObject* create(int, const char*const*) {
		return (new SRPT);
	}
} class_drop_tail;

void SRPT::reset()
{
	Queue::reset();
}

int 
SRPT::command(int argc, const char*const* argv) 
{
	if (argc==2) {
		if (strcmp(argv[1], "printstats") == 0) {
			print_summarystats();
			return (TCL_OK);
		}
 		if (strcmp(argv[1], "shrink-queue") == 0) {
 			shrink_queue();
 			return (TCL_OK);
 		}
	}
	if (argc == 3) {
		if (!strcmp(argv[1], "packetqueue-attach")) {
			delete q_;
			if (!(q_ = (PacketQueue*) TclObject::lookup(argv[2])))
				return (TCL_ERROR);
			else {
				pq_ = q_;
				return (TCL_OK);
			}
		}
	}
	return Queue::command(argc, argv);
}

void SRPT::enque(Packet* p)
{
	if (summarystats) {
                Queue::updateStats(qib_?q_->byteLength():q_->length());
	}

	int qlimBytes = qlim_ * mean_pktsize_;
	if ((!qib_ && (q_->length()) >= qlim_) ||
  	(qib_ && (q_->byteLength() + hdr_cmn::access(p)->size()) >= qlimBytes)){
		// if the queue would overflow if we added this packet...
                // Drop highest priority packet
                Packet *max_pp = p;
                int max_prio = 0;
                q_->enque(p);
                q_->resetIterator();
                for (Packet *pp = q_->getNext(); pp != 0; pp = q_->getNext()) {
                  if (!qib_ || ( q_->byteLength() - hdr_cmn::access(pp)->size() < qlimBytes)) {
                    hdr_ip* h = hdr_ip::access(pp);
                    int prio = h->prio();
                     if (prio >= max_prio) {
                       max_pp = pp;
                       max_prio = prio;
                      }
                   }
                }
                q_->remove(max_pp);
                drop(max_pp);   
	  } else {
                if(debugme_) {
                  hdr_ip* h = hdr_ip::access(p);
                  printf("%lf: %d: Enqueuing packet (%d, %d, %lld, %d)\n", Scheduler::instance().clock(), queueid_,  h->flowid(), getSeqNo(p), h->prio(), hdr_cmn::access(p)->size()); 
                }
		q_->enque(p);
	  }
}

//AG if queue size changes, we drop excessive packets...
void SRPT::shrink_queue() 
{
        int qlimBytes = qlim_ * mean_pktsize_;
	if (debug_)
		printf("shrink-queue: time %5.2f qlen %d, qlim %d\n",
 			Scheduler::instance().clock(),
			q_->length(), qlim_);
        while ((!qib_ && q_->length() > qlim_) || 
            (qib_ && q_->byteLength() > qlimBytes)) {
                if (drop_front_) { /* remove from head of queue */
                        Packet *pp = q_->deque();
                        drop(pp);
                } else {
                        Packet *pp = q_->tail();
                        q_->remove(pp);
                        drop(pp);
                }
        }
}

Packet* SRPT::deque()
{
        if (summarystats && &Scheduler::instance() != NULL) {
                Queue::updateStats(qib_?q_->byteLength():q_->length());
        }
        //finding highest priority packet
        q_->resetIterator();
        Packet *p = q_->getNext();
        int highest_prio_;
        if (p != 0)
          highest_prio_ = hdr_ip::access(p)->prio();
        else
          return 0;
        for (Packet *pp = q_->getNext(); pp != 0; pp = q_->getNext()) {
          hdr_ip* h = hdr_ip::access(pp);
          int prio = h->prio();
          //deque from the head
          if (prio < highest_prio_) {
            p = pp;
            highest_prio_ = prio;
          }
        }
        //keeping order
        q_->resetIterator();
        hdr_ip* hp = hdr_ip::access(p);
        for (Packet *pp = q_->getNext(); pp != p; pp = q_->getNext()) {
          hdr_ip* h = hdr_ip::access(pp);
          if (h->saddr() == hp->saddr() 
          && h->daddr() == hp->daddr() 
          && h->flowid() == hp->flowid()) {
            p = pp;
            break;
          }
        }
        q_->remove(p);
        if(debugme_) {
          hdr_ip* h = hdr_ip::access(p);
          printf("%lf: %d: Dequeuing packet (%d, %d, %lld, %d)\n", Scheduler::instance().clock(), queueid_, h->flowid(), getSeqNo(p), h->prio(), hdr_cmn::access(p)->size()); 
        }
        return p;
}

void SRPT::print_summarystats()
{
	//double now = Scheduler::instance().clock();
        printf("True average queue: %5.3f", true_ave_);
        if (qib_)
                printf(" (in bytes)");
        printf(" time: %5.3f\n", total_time_);
}
