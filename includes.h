#ifndef _MAININCLUDES_H_INCLUDE_
#define _MAININCLUDES_H_INCLUDE_

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h> // write
#include <string.h> // memset
#include <stdlib.h> // atoi
#include <stdbool.h> // true, false
#include <err.h> //error handling
#include <errno.h> //also error handling
#include <pthread.h>
#include <math.h> // ceil
#include <limits.h> // Used for INT_MAX

#define BUFFER_SIZE 4096
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#endif
