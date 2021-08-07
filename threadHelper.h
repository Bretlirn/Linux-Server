#ifndef _MAINFUNCS_H_INCLUDE_
#define _MAINFUNCS_H_INCLUDE_

#include "includes.h"

/*
  function to check if a string is alphanumeric
  Return:
    1 - true
    0 - false
*/
int isAlphaNumeric(const char* check);

/*
  function to check if a string is a number like itoa()
  Return:
    1 - true
    0 - false
*/
int isStringNumber(const char* c);

/*
  function to find the index on a string given a substring
  Return:
    int - index at where the string ends (from start to check)
*/
int findIndexString (const char* checking, const char* check);

int continuable(int m, int n, const char* checking, const char* check);
#endif
