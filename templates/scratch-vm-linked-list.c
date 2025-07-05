#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-linked-list.h"

#include <assert.h>
#include <stdlib.h>
#endif

ScratchList ScratchList_Init(void) {
  ScratchList result;
  result.first = 0;
  result.last = 0;
  result.size = 0;

  return result;
}

void ScratchList_InsertFront(ScratchList* list, ScratchListNode* node) {
  ScratchListNode* first = list->first;
  node->prev = 0;
  node->next = list->first;
  if (first) {
    first->prev = node;
  }
  list->first = node;

  if (!list->last) {
    list->last = node;
  }

  ++list->size;
}

void ScratchList_InsertBack(ScratchList* list, ScratchListNode* node) {
  ScratchListNode* last = list->last;
  node->prev = list->last;
  node->next = 0;
  if (last) {
    last->next = node;
  }
  list->last = node;

  if (!list->first) {
    list->first = node;
  }

  ++list->size;
}

void ScratchList_InsertBefore(ScratchList* list, ScratchListNode* insert_before,
                              ScratchListNode* node) {
  ScratchListNode* prev = insert_before->prev;
  insert_before->prev = node;
  node->next = insert_before;

  node->prev = prev;
  if (prev) {
    prev->next = node;
  }

  if (list->first == insert_before) {
    list->first = node;
  }

  ++list->size;
}

void ScratchList_InsertAfter(ScratchList* list, ScratchListNode* insert_after,
                             ScratchListNode* node) {
  ScratchListNode* next = insert_after->next;
  insert_after->next = node;
  node->prev = insert_after;

  node->next = next;
  if (next) {
    next->prev = node;
  }

  if (list->last == insert_after) {
    list->last = node;
  }

  ++list->size;
}

void ScratchList_InsertSorted(ScratchList* list, ScratchListNode* node,
                              ScratchListNodeComparator comparator) {
  (void)list;
  (void)node;
  (void)comparator;

  if (list->size == 0) {
    list->first = node;
    list->last = node;
    node->prev = 0;
    node->next = 0;

    ++list->size;

    return;
  }

  for (ScratchListNode* cur_node = list->first; cur_node != 0;
       cur_node = cur_node->next) {
    if (comparator(cur_node, node) >= 0) {
      ScratchList_InsertBefore(list, cur_node, node);
      return;
    }
  }

  ScratchList_InsertBack(list, node);
}

void ScratchList_Remove(ScratchList* list, ScratchListNode* node) {
  ScratchListNode* prev = node->prev;
  ScratchListNode* next = node->next;

  if (prev) {
    prev->next = next;
  }

  if (next) {
    next->prev = prev;
  }

  if (list->first == node) {
    list->first = next;
  }

  if (list->last == node) {
    list->last = prev;
  }

  --list->size;
}

size_t ScratchList_GetSize(ScratchList* list) { return list->size; }

ScratchListNode* ScratchList_GetFirst(ScratchList* list) { return list->first; }

ScratchListNode* ScratchList_GetNext(ScratchList* list,
                                     ScratchListNode* cur_node) {
  (void)list;
  return cur_node->next;
}
