//-----------------------------------------------------------------------------
// Queue.h
// Header file for Queue ADT
// Borrowed from Professor Patrick Tantalo's CSE101 materials
// https://classes.soe.ucsc.edu/cse101/Fall19/Examples/C/Queue/
//-----------------------------------------------------------------------------

#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_


// Exported type --------------------------------------------------------------
typedef struct QueueObj* Queue;


// Constructors-Destructors ---------------------------------------------------

// newQueue()
// Returns reference to new empty Queue object.
Queue newQueue(void);

//Non default constructor
Queue mutexQueue(pthread_mutex_t* mutex);

// freeQueue()
// Frees all heap memory associated with Queue *pQ, and sets *pQ to NULL.
void freeQueue(Queue* pQ);


// Access functions -----------------------------------------------------------

// getLength()
// Returns the length of Q.
int getLength(Queue Q);

// isEmpty()
// Returns true (1) if Q is empty, otherwise returns false (0)
int isEmpty(Queue Q);

//getMutex()
//gets the pthread_mutex_t variable
pthread_mutex_t* getMutex(Queue Q);

//getCond()
//gets the pthread_cond_t variable


// Manipulation procedures ----------------------------------------------------

// Enqueue()
// Places new data element at the end of Q
void Enqueue(Queue Q, int data);

// Dequeue()
// Deletes element at front of Q
// Pre: !isEmpty(Q)
int Dequeue(Queue Q);

#endif
