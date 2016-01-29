/**
 *
 * Start of channel implementation for UNIX
 *
 * Send, receive, bind, unbind atre implemented according to SPIN paper algorithms
 * Select operation is missing
 *
 * Absolutely no guarantees with this code, nevcer been tested by me
 *
 * @author jonl, based on code from Andrew Bell
 *
 * 26.04.2013
 *
 */


#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "../Collections/IteratedList.h"
#include "../GC/GC_mem.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// channel struct stuff
typedef enum { CHAN_IN, CHAN_OUT } chan_dir;

extern pthread_mutex_t conn_op_mutex;	// to prevent connect during disconnect and vice versa

typedef struct Channel chan_s, *Channel_PNTR;
typedef Channel_PNTR chan_id;
struct Channel {
	void (*decRef)(Channel_PNTR pntr); // GC decRef
	chan_dir direction;	// for error checking in bind, etc.
	size_t typesize;	// how large the buffer is
	void* buffer;		// pointer to data to send/receive
	bool ready;		// ready flag
	bool nd_received;	// used by select
	IteratedList_PNTR connections; 	// list of type Channel_PNTR, channels we're connected to
	pthread_mutex_t mutex;	// for locking the channel
//	my_sem_t conns_sem;	        // connections available mutex
//	my_sem_t blocked;	    	// block component if waiting for other channel
//	my_sem_t actually_received;	// make sure data can't be changed until after a receive has completed
};



//typedef Channel_PNTR chan_id; // channel pointer is unique, let's use it as id (remove when refactoring)

// select struct
struct select_struct
{
    int nchans;                 // number of channels
    Channel_PNTR *chans;             // array of channels for which guard conditions are satisfied
    void *buffer;               // ptr to the receiving buffer of appropriate size
    bool have_default;  // do we have default clause
};

// typedef enum {
//   BAD_CHAN_EVENT = 1,                        // channel index is out of range or its usage flags are not set
//   CHAN_RECV_NODATA_EVENT,                    // blocked receiver process woken without data to receive (interrupt woke up rcvr)
//   CHAN_RECV_BUFF_EMPTY_EVENT,                // returned by dequeue and send when q is empty
//   CHAN_RECV_EVENT,                           // successful receive from channel
//   CHAN_SEND_EVENT,                           // successful send over channel
//   RADIO_SEND_EVENT,                          // radio has sent data
//   RADIO_RECV_EVENT,                          // radio has received data
//   CHAN_CONNECT_EVENT,                        // channel has been connected
//   PUBLIC_CHAN_QUERY_EVENT,                   // query remote node about its public channels
//   NEIGHBOUR_QUERY_EVENT,                     // query remote node about its neighbours
//   GENERAL_SUCCESS_EVENT,                     // general success inidicator
//   GENERAL_EXCEPTION_EVENT                    // general exception inidcator
// } inceos_event_t;


// // Exception events which can be caught in Insense programs
// typedef enum {
//   // exceptions start at GENERAL_EXCEPTION_EVENT == 1+GENERAL_SUCCESS_EVENT
//   OperationFailedException = GENERAL_EXCEPTION_EVENT,
//   // for ASYNCH send code
//   ChannelBufferFullException,                 // thrown by send when local (and connected channel) queue buffers are full
//   DivisionByZeroException,
//   // For inter-node channels
//   SendStatusUnknownException,                 // status of of SYNCH send over inter-node channel is unknown, no ACK received
//   DuplicateLCNException,                      // two channels published with the same LCN on a single node
//   NodesUnreachableException,                  // there is no route to the remote node
//   ChannelsUnknownException,                   // the specified remote channel has not been published on the remote node
//   IncompatibleChannelsException,              // two channels are incomaptible for binding to take place
//   BindStatusUnknownException                  // status of binding two inter-node channels together is unknown, no ACK received
// } inceos_exception_t;


// channel functions
extern Channel_PNTR channel_create(chan_dir direction, int typesize, bool contains_pointers);
extern bool channel_bind(Channel_PNTR id1, Channel_PNTR id2);
extern void channel_unbind(Channel_PNTR id);
extern int channel_select(struct select_struct *s);
extern int channel_send(Channel_PNTR id, void *buffer, void *ex_handler); // ex_handler is an exception handler used in InceOS
extern int channel_receive(Channel_PNTR id, void *buffer, bool in_ack_after);
extern int channel_multicast_send(Channel_PNTR id, void *buffer);
extern void remoteAnonymousUnbind_proc(Channel_PNTR id, void* var);


#endif /* CHANNEL_H_ */
