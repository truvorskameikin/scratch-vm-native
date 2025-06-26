#ifndef __SCRATCH_VM_INCLUDE_BUFFERED_LINKED_LIST_H__
#define __SCRATCH_VM_INCLUDE_BUFFERED_LINKED_LIST_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-memory.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchBufferedListNode {
  ScratchMemoryBuffer buffer;
  struct ScratchBufferedListNode* next;
  struct ScratchBufferedListNode* prev;
} ScratchBufferedListNode;

typedef struct ScratchBufferedList {
  size_t single_node_size;
  size_t num_initial_nodes;
  size_t num_next_buffer_nodes;
  ScratchBufferedListNode* first;
  ScratchBufferedListNode* last;
  ScratchBufferedListNode* first_empty;
  size_t size;
  size_t capacity;
  size_t num_managed_buffers;
} ScratchBufferedList;

ScratchBufferedList ScratchBufferedList_Init(size_t single_node_size,
                                             size_t num_initial_nodes,
                                             size_t num_next_buffer_nodes,
                                             ScratchMemoryBuffer* first_buffer);
ScratchBufferedListNode* ScratchBufferedList_InsertNew(
    ScratchBufferedList* list);
size_t ScratchBufferedList_Size(ScratchBufferedList* list);
size_t ScratchBufferedList_Capacity(ScratchBufferedList* list);
void ScratchBufferedList_Remove(ScratchBufferedList* list,
                                ScratchBufferedListNode* node);
ScratchBufferedListNode* ScratchBufferedList_GetFirst(
    ScratchBufferedList* list);
ScratchBufferedListNode* ScratchBufferedList_GetNext(
    ScratchBufferedList* list, ScratchBufferedListNode* cur_node);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_BUFFERED_LINKED_LIST_H__
