#ifndef __SCRATCH_VM_INCLUDE_LINKED_LIST_H__
#define __SCRATCH_VM_INCLUDE_LINKED_LIST_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-memory.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchListNode {
  struct ScratchListNode* prev;
  struct ScratchListNode* next;
} ScratchListNode;

typedef struct ScratchList {
  ScratchListNode* first;
  ScratchListNode* last;
  size_t size;
} ScratchList;

typedef int (*ScratchListNodeComparator)(ScratchListNode* lhv,
                                         ScratchListNode* rhv);

ScratchList ScratchList_Init(void);
void ScratchList_InsertFront(ScratchList* list, ScratchListNode* node);
void ScratchList_InsertBack(ScratchList* list, ScratchListNode* node);
void ScratchList_InsertBefore(ScratchList* list, ScratchListNode* insert_before,
                              ScratchListNode* node);
void ScratchList_InsertAfter(ScratchList* list, ScratchListNode* insert_after,
                             ScratchListNode* node);
void ScratchList_InsertSorted(ScratchList* list, ScratchListNode* node,
                              ScratchListNodeComparator comparator);
void ScratchList_Remove(ScratchList* list, ScratchListNode* node);
size_t ScratchList_GetSize(ScratchList* list);
ScratchListNode* ScratchList_GetFirst(ScratchList* list);
ScratchListNode* ScratchList_GetNext(ScratchList* list,
                                     ScratchListNode* cur_node);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_LINKED_LIST_H__
