#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-buffered-linked-list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#endif

static ScratchBufferedListNode* PlaceEmptyNodes(
    ScratchMemoryBuffer* memory_buffer, size_t single_node_size,
    size_t num_initial_nodes) {
  void* buffer = memory_buffer->buffer;

  ScratchBufferedListNode* cur_node = (ScratchBufferedListNode*)buffer;
  ScratchBufferedListNode* result = cur_node;

  buffer = (char*)buffer + single_node_size;

  cur_node->buffer = *memory_buffer;
  cur_node->next = 0;

  for (size_t i = 1; i < num_initial_nodes; ++i) {
    ScratchBufferedListNode* node = (ScratchBufferedListNode*)buffer;
    buffer = (char*)buffer + single_node_size;

    node->buffer = *memory_buffer;

    cur_node->next = node;
    cur_node = node;
  }

  return result;
}

extern ScratchMemoryBuffer ScratchMemory_Alloc(void* unmanaged_buffer,
                                               size_t buffer_size);

ScratchBufferedList ScratchBufferedList_Init(
    size_t single_node_size, size_t num_initial_nodes,
    size_t num_next_buffer_nodes, ScratchMemoryBuffer* first_buffer) {
  ScratchBufferedList result;
  result.single_node_size = single_node_size;
  result.num_initial_nodes = num_initial_nodes;
  result.num_next_buffer_nodes = num_next_buffer_nodes;
  result.capacity = result.num_initial_nodes;
  result.size = 0;

  ScratchMemoryBuffer buffer_to_use;

  if (first_buffer) {
    assert(result.num_initial_nodes <=
               first_buffer->size / result.single_node_size &&
           "If buffer is passed to buffered list, ensure to provide enough "
           "room for num_initial_nodes.");

    buffer_to_use = *first_buffer;
  } else {
    buffer_to_use =
        ScratchMemory_Alloc(/*unmanaged_buffer=*/0,
                            result.num_initial_nodes * result.single_node_size);
  }

  result.num_managed_buffers = 0;
  if (buffer_to_use.type == kScratchMemoryBufferManaged) {
    result.num_managed_buffers = 1;
  }

  // No used nodes.
  result.first = 0;
  result.last = 0;

  result.first_empty = PlaceEmptyNodes(&buffer_to_use, result.single_node_size,
                                       result.num_initial_nodes);

  return result;
}

ScratchBufferedListNode* ScratchBufferedList_InsertNew(
    ScratchBufferedList* list) {
  if (!list->first_empty) {
    ScratchMemoryBuffer buffer_to_use = ScratchMemory_Alloc(
        /*unmanaged_buffer=*/0,
        list->num_next_buffer_nodes * list->single_node_size);

    ++list->num_managed_buffers;
    list->capacity += list->num_next_buffer_nodes;

    list->first_empty = PlaceEmptyNodes(&buffer_to_use, list->single_node_size,
                                        list->num_initial_nodes);
  }

  ScratchBufferedListNode* result = list->first_empty;
  list->first_empty = list->first_empty->next;

  result->prev = 0;
  result->next = 0;

  if (list->last) {
    result->prev = list->last;
    list->last->next = result;
    list->last = result;
  } else {
    list->first = result;
    list->last = result;
  }

  ++list->size;

  return result;
}

size_t ScratchBufferedList_Size(ScratchBufferedList* list) {
  return list->size;
}

size_t ScratchBufferedList_Capacity(ScratchBufferedList* list) {
  return list->capacity;
}

void ScratchBufferedList_Remove(ScratchBufferedList* list,
                                ScratchBufferedListNode* node) {
  ScratchBufferedListNode* prev = node->prev;
  ScratchBufferedListNode* next = node->next;

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

  // Add to empty.
  node->next = list->first_empty;
  list->first_empty = node;
}

ScratchBufferedListNode* ScratchBufferedList_GetFirst(
    ScratchBufferedList* list) {
  return list->first;
}

ScratchBufferedListNode* ScratchBufferedList_GetNext(
    ScratchBufferedList* list, ScratchBufferedListNode* cur_node) {
  (void)list;

  if (!cur_node) {
    return 0;
  }

  return cur_node->next;
}
