#include "thread.h"

/*
 * client_connect takes a port number and establishes a connection as a client.
 * connectport: port number of server to connect to
 * returns: valid socket if successful, -1 otherwise
 */
int client_connect(uint16_t connectport) {
    int connfd;
    struct sockaddr_in servaddr;

    connfd=socket(AF_INET,SOCK_STREAM,0);
    if (connfd < 0)
        return -1;
    memset(&servaddr, 0, sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(connectport);

    /* For this assignment the IP address can be fixed */
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    if(connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        return -1;
    return connfd;
}

/*
 * bridge_connections send up to 100 bytes from fromfd to tofd
 * fromfd, tofd: valid sockets
 * returns: number of bytes sent, 0 if connection closed, -1 on error
 */
 int bridge_connections(int fromfd, int tofd) {
     char recvline[100];
     memset(&recvline,0,sizeof(recvline));
     int n = recv(fromfd, recvline, 100, 0);
     if (n < 0) {
         printf("connection error receiving\n");
         return -1;
     } else if (n == 0) {
         printf("receiving connection ended\n");
         return 0;
     }
     n = send(tofd, recvline, n, 0);
     if (n < 0) {
         printf("connection error sending\n");
         return -1;
     } else if (n == 0) {
         printf("sending connection ended\n");
         return 0;
     }
     return n;
 }

 /*
  * bridge_loop forwards all messages between both sockets until the connection
  * is interrupted. It also prints a message if both channels are idle.
  * sockfd1, sockfd2: valid sockets
  */
 void bridge_loop(int sockfd1, int sockfd2) {
     fd_set set;
     struct timeval timeout;

     int fromfd, tofd;
     while(1) {
         // set for select usage must be initialized before each select call
         // set manages which file descriptors are being watched
         FD_ZERO (&set);
         FD_SET (sockfd1, &set);
         FD_SET (sockfd2, &set);

         // same for timeout
         // max time waiting, 5 seconds, 0 microseconds
         timeout.tv_sec = 5;
         timeout.tv_usec = 0;

         // select return the number of file descriptors ready for reading in set
         switch (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
             case -1:
                 //printf("error during select, exiting\n");
                 dprintf(sockfd1,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
                 return;
             case 0:
                 //printf("both channels are idle, waiting again\n");
                 dprintf(sockfd1,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
                 continue;
             default:
                 if (FD_ISSET(sockfd1, &set)) {
                     fromfd = sockfd1;
                     tofd = sockfd2;
                 } else if (FD_ISSET(sockfd2, &set)) {
                     fromfd = sockfd2;
                     tofd = sockfd1;
                 } else {
                     //printf("this should be unreachable\n");
                     return;
                 }
         }
         if (bridge_connections(fromfd, tofd) <= 0)
             return;
     }
 }

void* thread_start(void* data){

    printf("Making a thread\n");
    ThreadArg* argp = (ThreadArg*) data;
    Queue* QueueList = argp->workList;
    pthread_mutex_t* mut = getMutex(*QueueList);
    int lbClientfd;
    int l;
    while(true){
      //pthread_mutex_lock(mut);
      if((l = pthread_mutex_lock(mut))!= 0)
        printf("%i", l);
      printf("In Cond Wait thread for %i\n", argp->identifier);
      while(isEmpty(*QueueList)){
        pthread_cond_wait(argp->thrCond, mut);
        printf("Moved out of wait\n");
      }
      printf("Thread %i doing stuff\n", argp->identifier);
      int lbServerfd = Dequeue(*QueueList);
      pthread_mutex_unlock(mut);

      //getting best Server
      pthread_mutex_lock(argp->healthChecker->hcMut);
      printf("*argp->healthChecker->serverUse %i\n", *argp->healthChecker->serverUse);
      if (*argp->healthChecker->serverUse == -1){
        dprintf(lbServerfd,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
        pthread_mutex_unlock(argp->healthChecker->hcMut);
        continue;
      }
      else{
        if ((lbClientfd = client_connect(argp->healthChecker->serverPorts[*argp->healthChecker->serverUse].port)) < 0){
          dprintf(lbServerfd,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
          pthread_mutex_unlock(argp->healthChecker->hcMut);
          continue;
        }
        pthread_mutex_unlock(argp->healthChecker->hcMut);
        //printf("Bridge connected\n");
        bridge_loop(lbServerfd, lbClientfd);
        close(lbServerfd);
        close(lbClientfd);
      }
    }
    //printf("I should not get here in worker thread\n");
    return NULL;
}

void* single_hc(void*data){
  int serverfd;
  int totalBytes;
  int readBytes;
  char HCBuffer[BUFFER_SIZE];


  char s_inc[10];
  char s_tot[10];
  char s_err[10];
  char s_dump[BUFFER_SIZE];

  fd_set set;
  struct timeval timeout;
  // same for timeout
  // max time waiting, 5 seconds, 0 microseconds
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  struct ConnectObj* argp = (struct ConnectObj*) data;
  argp->alive = 2;
  if ((serverfd = client_connect(argp->port)) < 0){
    argp->alive = 0;
    return NULL;
  }
  FD_ZERO(&set);
  FD_SET(serverfd, &set);

  char* HCstr = "GET /healthcheck HTTP/1.1\r\nContent-Length: 0\r\n\r\n";
  int writebytes = 0;
  while(writebytes < (signed int) strlen(HCstr)){
    writebytes += send(serverfd, HCstr+writebytes, strlen(HCstr) - writebytes, 0);
  }
  do{
    if(select(FD_SETSIZE, &set, NULL, NULL, &timeout) > 0){
      readBytes = 0;
      readBytes = recv(serverfd, HCBuffer + readBytes, BUFFER_SIZE - readBytes, 0);
      totalBytes = readBytes;
      while(readBytes > 0){
        readBytes = recv(serverfd, HCBuffer + readBytes, BUFFER_SIZE - readBytes,0);
        totalBytes += readBytes;
      }
      break;
    }
    else{
      argp->alive = 0;
      close(serverfd);
      return NULL;
    }
  }while(true);
  //printf("%s\n", HCBuffer);
  close(serverfd);
  int imstupidok = sscanf(HCBuffer, "%s %s %s\n%s %s\r\n\r\n%s\n%s %s",s_dump, s_err, s_dump, s_dump, s_dump, s_inc, s_tot, s_dump);
  //printf("%i --- %s --- %s --- %s\n", imstupidok, s_err, s_inc, s_tot);
  if (imstupidok != 7 ){
    return NULL;
  }
  if (strcmp(s_err, "200") == 0){
    argp->inc = atoi(s_inc);
    argp->total = atoi(s_tot);
    argp->percentage = 0;//(double)(argp->total - argp->inc) / (double)(argp->total);
    argp->alive = 1;
    return NULL;
  }
  else{
    argp->alive = 0;
    return NULL;
  }
}

void* hc_thread(void* data){
  HCArgs* argp = (HCArgs*) data;
  struct timeval  now;            /* time when we started waiting        */
  struct timespec timeout;        /* timeout value for the wait function */
  int firstTime = 1;

  pthread_mutex_lock(argp->healthChecker->hcMut);
  while(true){
    //printf("Going through the loop in HC\n");
    gettimeofday(&now, NULL);
    /* prepare timeout value */
    timeout.tv_sec = now.tv_sec + (argp->time/1000000);
    timeout.tv_nsec = now.tv_usec * 1000 + ((argp->time%1000000) * 1000);

    //printf("Sent: %i\nCheck: %i\n", argp->healthChecker->sent , argp->check);
    //printf("argp->healthChecker->sent mod argp->check = %i\n", argp->healthChecker->sent % argp->check);
    if(firstTime != 1){
      pthread_cond_timedwait(argp->healthChecker->hcCond, argp->healthChecker->hcMut, &timeout);
    }
    firstTime = 0;
    /*gettimeofday(&now, NULL);
    if (now.tv_sec <= timeout.tv_sec){
      //printf("----->Left mutex due to time\n");
    }
    else{
      //printf("----->Left mutex due to check amount\n");
    }*/

    pthread_t healthCheck_thread[argp->healthChecker->serverPortSize];
    for(int i = 0; i < argp->healthChecker->serverPortSize; i++){
      argp->healthChecker->serverPorts[i].inc = INT_MAX;
      argp->healthChecker->serverPorts[i].total = INT_MAX;
      pthread_create(&healthCheck_thread[i], NULL, single_hc, &argp->healthChecker->serverPorts[i]);
    }

    for(int i = 0; i < argp->healthChecker->serverPortSize; i++){
      pthread_join(healthCheck_thread[i], NULL);
    }
    *argp->healthChecker->serverUse = -1;


    int lowest = INT_MAX;
    *argp->healthChecker->serverUse = -1;
    for(int i = 0; i < argp->healthChecker->serverPortSize; i++){
      printf("%i --- %i\n", i, argp->healthChecker->serverPorts[i].total);
      if(argp->healthChecker->serverPorts[i].total < lowest){
        *argp->healthChecker->serverUse = i;
        lowest = argp->healthChecker->serverPorts[i].total;
      }
    }
    //printf("Best Server: %i\n", *argp->healthChecker->serverUse);
  }
  //printf("I should not get here in HC thread\n");
  return NULL;
}
