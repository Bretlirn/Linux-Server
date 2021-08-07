/*
 * List.c
 *
 *  Created on: Oct 18, 2018
 *      Author: vchen18
 */
#include "List.h"

// structs --------------------------------------------------------------------

// private ListNodeObj type
typedef struct ListNodeObj{
   struct httpObject data;
   struct ListNodeObj* prev;
   struct ListNodeObj* next;
} ListNodeObj;

// private Node type
typedef ListNodeObj* ListNode;

typedef struct ListObj{
	ListNode head;
	ListNode tail;
	int length;
  pthread_mutex_t * mut;
} ListObj;

// Constructors-Destructors ---------------------------------------------------

// newListNode()
// Returns reference to new ListNode object. Initializes next and data fields.
// Private.
ListNode newListNode(struct httpObject data){
   ListNode N = malloc(sizeof(ListNodeObj));
   N->data = data;
   N->next = NULL;
   return(N);
}

// freeListNode()
// Frees heap memory pointed to by *pN, sets *pN to NULL.
// Private.
void freeListNode(ListNode* pN){

   if( pN!=NULL && *pN!=NULL ){
     free(*pN);
     *pN = NULL;
   }
}

// Constructors-Destructors ---------------------------------------------------
List newList(void){
	List L;
	L = malloc(sizeof(ListObj));
	L->head = L->tail = NULL;
	L->length = 0;
  L->mut = NULL;
	return(L);
}

List mutexList(pthread_mutex_t* newMut){
  List L;
  L = malloc(sizeof(ListObj));
  L->head = L->tail = NULL;
  L->length = 0;
  L->mut = newMut;
  return(L);
}

void freeList(List* pL){
	if(pL!=NULL && *pL!=NULL) {
		clear(*pL);
		free(*pL);
		*pL = NULL;
	}
}
// Access functions -----------------------------------------------------------
int length(List L){
	if (L == NULL)
	{
	      printf("List Error: calling length() on NULL List reference\n");
	      exit(1);
	}
	return L->length;
}

struct httpObject front(List L){
	if (L == NULL){
	      printf("List Error: calling front() on NULL List reference\n");
	      exit(1);
	}
	return L->head->data;
}

struct httpObject back(List L){
	if (L == NULL){
	      printf("List Error: calling back() on NULL List reference\n");
	      exit(1);
	}
	return L->tail->data;
}

pthread_mutex_t* getListMutex(List L){
  return (L->mut);
}
// Manipulation procedures ----------------------------------------------------
void clear(List L){
  pthread_mutex_lock(L->mut);
	if (L->head == L->tail && L->head == NULL){
		L->length = 0;
		return;
	}
	ListNode N = L->head;
	ListNode T = NULL;
	while (N != NULL){
		if (N != L->tail){
			T = N->next;
		}
		else{
			T = NULL;
		}
		freeListNode(&N);
		N = T;

	}
	L->head = L->tail  = NULL;
	L->length = 0;
  pthread_mutex_unlock(L->mut);
}

void prepend(List L, struct httpObject data){
	ListNode N = newListNode(data);
  pthread_mutex_lock(L->mut);
	if (L == NULL){
		freeListNode(&N);
		printf("List Error: calling prepend() on NULL List reference\n");
		exit(1);
	}
	if (L->length == 0){
		L->head = L->tail = N;
		L->length = L->length + 1;
	}
	else{
		L->head->prev = N;
		N->next = L->head;
		L->head = N;
		L->length = L->length + 1;
	}
  pthread_mutex_unlock(L->mut);
}

void append(List L, struct httpObject data){
	ListNode N = newListNode(data);
  pthread_mutex_lock(L->mut);
	if (L == NULL){
		freeListNode(&N);
		printf("List Error: calling append() on NULL List reference\n");
		exit(1);
	}
	if (L->length == 0){
		L->head = L->tail = N;
		L->length = L->length + 1;
	}
	else{
		L->tail->next = N;
		N->prev = L->tail;
		L->tail = N;
		L->length = L->length + 1;
	}
  pthread_mutex_unlock(L->mut);
}

void deleteFront(List L){
	if (L == NULL){
	      printf("List Error: calling deleteFront() on NULL List reference\n");
	      exit(1);
	}

  pthread_mutex_lock(L->mut);
	if (L->length > 0){
		ListNode T = L->head;
		if(L->head != L->tail){
			L->head = L->head->next;
			L->head->prev = NULL;
		}
    else{
      L->head = L->tail = NULL;
    }
		freeListNode(&T);
		L->length = L->length - 1;
	}
  pthread_mutex_unlock(L->mut);
}

void deleteBack(List L){
	if (L == NULL){
	      printf("List Error: calling deleteFront() on NULL List reference\n");
	      exit(1);
	}

  pthread_mutex_lock(L->mut);
	if (L->length > 0){
		ListNode T = L->tail;
		if(L->head != L->tail){
			L->tail = L->tail->prev;
			L->tail->next = NULL;
		}
		freeListNode(&T);
		L->length = L->length - 1;
	}

  pthread_mutex_unlock(L->mut);
}

// Other operations -----------------------------------------------------------
/*void printList(FILE* out, List L){
	ListNode N = L->head;
	while (N != NULL){
		fprintf(out, "% ", N->data);
		N = N->next;
	}
}*/

int fileNameIn(List L, char* fileName){
  ListNode N = L->head;
  while (N != NULL){
    if (strcmp(fileName, N->data.filename) == 0){
      return 1;
    }
    N = N->next;
  }
  return 0;
}
