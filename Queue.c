//-----------------------------------------------------------------------------
// Queue.c
// Implementation file for Queue ADT
// Borrowed from Professor Patrick Tantalo's CSE101 materials
// https://classes.soe.ucsc.edu/cse101/Fall19/Examples/C/Queue/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Queue.h"

// structs --------------------------------------------------------------------

// private NodeObj type
typedef struct NodeObj{
   int data;
   struct NodeObj* next;
} NodeObj;

// private Node type
typedef NodeObj* Node;

// private QueueObj type
typedef struct QueueObj{
   Node front;
   Node back;
   int length;
   pthread_mutex_t * mut;
} QueueObj;


// Constructors-Destructors ---------------------------------------------------

// newNode()
// Returns reference to new Node object. Initializes next and data fields.
// Private.
Node newNode(int data){
   Node N = malloc(sizeof(NodeObj));
   N->data = data;
   N->next = NULL;
   return(N);
}

// freeNode()
// Frees heap memory pointed to by *pN, sets *pN to NULL.
// Private.
void freeNode(Node* pN){
   if( pN!=NULL && *pN!=NULL ){
      free(*pN);
      *pN = NULL;
   }
}

// newQueue()
// Returns reference to new empty Queue object.
Queue newQueue(void){
   Queue Q;
   Q = malloc(sizeof(QueueObj));
   Q->front = Q->back = NULL;
   Q->length = 0;
   pthread_mutex_init(Q->mut, NULL);
   return(Q);
}

Queue mutexQueue(pthread_mutex_t* mutex){
   Queue Q;
   Q = malloc(sizeof(QueueObj));
   Q->front = Q->back = NULL;
   Q->length = 0;
   Q->mut = mutex;
   return(Q);
}



// freeQueue()
// Frees all heap memory associated with Queue *pQ, and sets *pQ to NULL.S
void freeQueue(Queue* pQ){
   if(pQ!=NULL && *pQ!=NULL) {
      while( !isEmpty(*pQ) ) {
         Dequeue(*pQ);
      }
      free(*pQ);
      *pQ = NULL;
   }
}


// Access functions -----------------------------------------------------------

// getLength()
// Returns the length of Q.
int getLength(Queue Q){
   if( Q==NULL ){
      printf("Queue Error: calling getLength() on NULL Queue reference\n");
      exit(1);
   }
   return(Q->length);
}

// isEmpty()
// Returns true (1) if Q is empty, otherwise returns false (0)
int isEmpty(Queue Q){
   if( Q==NULL ){
      printf("Queue Error: calling isEmpty() on NULL Queue reference\n");
      exit(1);
   }
   return(Q->length==0);
}

pthread_mutex_t* getMutex(Queue Q){
  if( Q==NULL ){
     printf("Queue Error: calling getMutex() on NULL Queue reference\n");
     exit(1);
  }
  return (Q->mut);
}

// Manipulation procedures ----------------------------------------------------

// Enqueue()
// Places new data element at the end of Q
void Enqueue(Queue Q, int data)
{
   Node N = newNode(data);

   if( Q==NULL ){
      printf("Queue Error: calling Enqueue() on NULL Queue reference\n");
      exit(1);
   }
   if( isEmpty(Q) ) {
      Q->front = Q->back = N;
   }else{
      Q->back->next = N;
      Q->back = N;
   }
   Q->length++;
}

// Dequeue()
// Deletes element at front of Q
// Pre: !isEmpty(Q)
int Dequeue(Queue Q){
  int data;
  if( Q==NULL ){
      printf("Queue Error: calling getFront() on NULL Queue reference\n");
      exit(1);
   }
   if( isEmpty(Q) ){
      printf("Queue Error: calling getFront() on an empty Queue\n");
      exit(1);
   }
   data = (Q->front->data);

   Node N = NULL;

   if( Q==NULL ){
      printf("Queue Error: calling Dequeue() on NULL Queue reference\n");
      exit(1);
   }
   if( isEmpty(Q) ){
      printf("Queue Error: calling Dequeue on an empty Queue\n");
      exit(1);
   }
   N = Q->front;
   if( getLength(Q)>1 ) {
      Q->front = Q->front->next;
   }else{
      Q->front = Q->back = NULL;
   }
   Q->length--;
   freeNode(&N);
   return data;
}
