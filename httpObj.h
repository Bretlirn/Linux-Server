#ifndef _HTTPOBJ_H_INCLUDE_
#define _HTTPOBJ_H_INCLUDE_

#include "includes.h"
/*
    Create some object 'struct' to keep track of all
    the components related to a HTTP message
    NOTE: There may be more member variables you would want to add
*/

struct  httpObject{
  /*
      Create some object 'struct' to keep track of all
      the components related to a HTTP message
      NOTE: There may be more member variables you would want to add
  */

  char method[5];         // PUT, HEAD, GET
  char filename[256];      // what is the file we are worried about
  char httpversion[9];    // HTTP/1.1
  ssize_t content_length; // example: 13
  int status_code;
  char buffer[BUFFER_SIZE];
};

#endif
