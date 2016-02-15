#ifndef ns_srpt_h
#define ns_srpt_h

#include <string.h>
#include "queue.h"
#include "config.h"

/*
 * A bounded, drop-tail queue
 */
class SRPT : public Queue {
  public:
	SRPT() { 
		q_ = new PacketQueue; 
		pq_ = q_;
		bind_bool("drop_front_", &drop_front_);
		bind_bool("summarystats_", &summarystats);
		bind_bool("queue_in_bytes_", &qib_);  // boolean: q in bytes?
		bind("mean_pktsize_", &mean_pktsize_);
                bind("debugme_", &debugme_);  
                bind("queueid_", &queueid_);  
		//		_RENAMED("drop-front_", "drop_front_");
	}
	~SRPT() {
		delete q_;
	}
  protected:
	void reset();
	int command(int argc, const char*const* argv); 
	void enque(Packet*);
	Packet* deque();
	void shrink_queue();	// To shrink queue and drop excessive packets.

	PacketQueue *q_;	/* underlying FIFO queue */
	int drop_front_;	/* drop-from-front (rather than from tail) */
	int summarystats;
	void print_summarystats();
	int qib_;       	/* bool: queue measured in bytes? */
	int mean_pktsize_;	/* configured mean packet size in bytes */
        int debugme_;
        int queueid_;
};

#endif
