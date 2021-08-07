/*
 * List.h
 *
 *  Created on: Oct 18, 2018
 *      Author: vchen18
 */

#ifndef _LIST_H_INCLUDE__
#define _LIST_H_INCLUDE__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>

#include "httpObj.h"

// Exported type --------------------------------------------------------------
typedef struct ListObj* List;

// Constructors-Destructors ---------------------------------------------------
List newList(void);
List mutexList(pthread_mutex_t* newMut);
void freeList(List* pL);
// Access functions -----------------------------------------------------------
int length(List L);
struct httpObject front(List L);
struct httpObject back(List L);
pthread_mutex_t* getListMutex(List L);
// Manipulation procedures ----------------------------------------------------
void clear(List L);
void moveFront(List L);
void moveBack(List L);
void prepend(List L, struct httpObject data);
void append(List L, struct httpObject data);

void deleteFront(List L);
void deleteBack(List L);

void delete(List L);
// Other operations -----------------------------------------------------------
//void printList(FILE* out, List L);
List copyList(List L);
int fileNameIn(List L, char* fileName);

#endif /* LIST_H_ */
