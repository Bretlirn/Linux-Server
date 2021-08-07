#ifndef _THREAD_H_INCLUDE_
#define _THREAD_H_INCLUDE_


#include "includes.h"
#include "threadHelper.h"
#include "Queue.h" // Queue datatype
#include "List.h" // List datatype
#include "httpObj.h"
/* structure for thread initilization arguments */

/*typedef struct file_list_args{
  char* readArray;
  int readRow;
  int readCol;
  char* writeArray;
  int writeRow;
  int writeCol;
  pthread_cond_t* fileCond;
  pthread_mutex_t* fileMut;
}FileArgs;*/

typedef struct file_guard{
  int row;
  int col;
  int* modifying;
  char* fileArray;
  pthread_cond_t* cond;
  pthread_mutex_t* mut;
}FileGuard;

struct ConnectObj{
  int identifier;
  int port;
  int inc;
  int total;
  double percentage;
  int alive;
};

typedef struct healthcheck_obj{
  pthread_cond_t* hcCond;
  pthread_mutex_t* hcMut;
  struct ConnectObj* serverPorts;
  int serverPortSize;
  int sent;
  int check;
  int* serverUse;
}HealthCheckObj;

typedef struct healthcheck_args{
  int time;
  int check;
  HealthCheckObj* healthChecker;
}HCArgs;


typedef struct thread_args {
    pthread_cond_t* thrCond;
    int identifier;
    Queue* workList;
    HealthCheckObj* healthChecker;
}ThreadArg;

/* function to be executed by the new thread */
void* thread_start(void* data);

void* hc_thread(void* data);

#endif
