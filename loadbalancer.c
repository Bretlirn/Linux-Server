#include "includes.h"
#include "thread.h"
#include "Queue.h"
#include "List.h"
#include "httpObj.h"

/*
 * server_listen takes a port number and creates a socket to listen on
 * that port.
 * port: the port number to receive connections
 * returns: valid socket if successful, -1 otherwise
 */
int server_listen(int port) {
    int listenfd;
    int enable = 1;
    struct sockaddr_in servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
        return -1;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
        return -1;
    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof servaddr) < 0)
        return -1;
    if (listen(listenfd, 500) < 0)
        return -1;
    return listenfd;
}


int main(int argc,char **argv) {
    int listenfd;

    int numThreads = 4;
    int healthCheckTime = INT_MAX;
    int listenPort = -1;
    struct ConnectObj* connectPorts = NULL;
    int connectPortSize = 0;
    if (argc < 3) {
        write(2, "missing arguments: usage %s port_to_connect port_to_listen\n", strlen("missing arguments: usage %s port_to_connect port_to_listen\n"));
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++){
      //For number of threads
      if (strcmp(argv[i], "-N") == 0){
        if (i + 1 >= argc){
          free(connectPorts);
          write(2, "-N with no value\n", strlen("-N with no value\n"));
          return EXIT_FAILURE;
        }
        else{
          i++;
          numThreads = atoi(argv[i]);
          if(numThreads < 1){
            free(connectPorts);
            write(2, "Negative port number\n", strlen("Negative port number\n"));
            return EXIT_FAILURE;
          }
        }
      }

      //For health check time
      else if (strcmp(argv[i], "-R") == 0){
        if (i + 1 >= argc){
          free(connectPorts);
          write(2, "-R with no value\n", strlen("-R with no value\n"));
          return EXIT_FAILURE;
        }
        else{
          i++;
          healthCheckTime = atoi(argv[i]);
          if(healthCheckTime < 1){
            free(connectPorts);
            write(2, "Negative port number\n", strlen("Negative port number\n"));
            return EXIT_FAILURE;
          }
        }
      }
      else{
        //First time finding a port #
        if (listenPort == -1){
          listenPort = atoi(argv[i]);
          if (listenPort < 0){
            free(connectPorts);
            write(2, "Negative port number\n", strlen("Negative port number\n"));
            return EXIT_FAILURE;
          }
        }
        //Other times finding a port #
        else{
          connectPorts = (struct ConnectObj*) realloc(connectPorts, sizeof(struct ConnectObj) * (connectPortSize + 1));
          connectPorts[connectPortSize].port = atoi(argv[i]);
          if (connectPorts[connectPortSize].port < 0){
            free(connectPorts);
            write(2, "Negative port number\n", strlen("Negative port number\n"));
            return EXIT_FAILURE;
          }
          connectPorts[connectPortSize].percentage = 1.0;
          connectPorts[connectPortSize].inc = 0;
          connectPorts[connectPortSize].total = 0;
          connectPorts[connectPortSize].identifier = connectPortSize + 1;
          connectPorts[connectPortSize].alive = 1; //for now
          connectPortSize++;
        }
      }
    }

    pthread_mutex_t queueMut;
    pthread_cond_t queueCond;

    pthread_cond_init(&queueCond, NULL);
    pthread_mutex_init(&queueMut, NULL);

    Queue mainQueue = mutexQueue(&queueMut);

    pthread_mutex_t hcMut;
    pthread_cond_t hcCond;

    pthread_cond_init(&hcCond, NULL);
    pthread_mutex_init(&hcMut, NULL);

    int serverToUse = -1;
    HealthCheckObj hc;
    hc.hcCond = &hcCond;
    hc.hcMut = &hcMut;
    hc.serverPorts = connectPorts;
    hc.serverPortSize = connectPortSize;
    hc.sent = 0;
    hc.check = healthCheckTime;
    hc.serverUse = &serverToUse;
    HCArgs hc_args;
    hc_args.healthChecker = &hc;
    hc_args.check = healthCheckTime;
    hc_args.time = 4000000;

    pthread_t healthCheck_thread;
    pthread_create(&healthCheck_thread, NULL, hc_thread, &hc_args);

    pthread_t thread_ids[numThreads];
    ThreadArg args[numThreads];
    for (int idx = 0; idx < numThreads; ++idx) {
        args[idx].thrCond = &queueCond;
        args[idx].identifier = idx;
        args[idx].workList = &mainQueue;
        args[idx].healthChecker = &hc;
        //printf("Making a thread in main\n");
        pthread_create(&thread_ids[idx], NULL, thread_start, &args[idx]);
        sleep(0.001);
    }


    //Connecting to a port to listen
    if ((listenfd = server_listen(listenPort)) < 0)
        err(1, "failed listening");
    int numReq = 0;
    int client_sockd;
    while (true) {
        //clearMsg(&message);
        printf("[+] server is waiting...\n");
        if((client_sockd = accept(listenfd, NULL, NULL)) < 0){
          printf("accept: %i", client_sockd);
          dprintf(client_sockd ,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
          continue;
        }
        //printf("Queue Length: %i\n", getLength(mainQueue));
        pthread_mutex_lock(&queueMut);
        Enqueue(mainQueue, client_sockd);
        pthread_cond_signal(&queueCond);
        pthread_mutex_unlock(&queueMut);
        numReq++;

        pthread_mutex_lock(&hcMut);
        if(numReq%healthCheckTime==0)
        {
            pthread_cond_signal(&hcCond);
        }
        pthread_mutex_unlock(&hcMut);

    }
    printf("I should not be here\n");
    free(connectPorts);
}
