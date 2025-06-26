#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-memory.h"

#include <assert.h>
#include <stdlib.h>
#endif

static size_t scratch_used_managed_memory = 0;
static size_t scratch_used_unmanaged_memory = 0;

ScratchMemoryBuffer ScratchMemory_Alloc(void* unmanaged_buffer,
                                        size_t buffer_size) {
  ScratchMemoryBuffer result;
  if (unmanaged_buffer) {
    result.type = kScratchMemoryBufferUnmanaged;
    result.buffer = unmanaged_buffer;
    result.size = buffer_size;

    scratch_used_unmanaged_memory += buffer_size;
  } else {
    result.type = kScratchMemoryBufferManaged;
    result.buffer = malloc(buffer_size);
    result.size = buffer_size;

    scratch_used_managed_memory += buffer_size;
  }

  return result;
}

ScratchMemoryBuffer ScratchMemory_AllocInplace(void* buffer,
                                               size_t buffer_size) {
  ScratchMemoryBuffer result;
  result.buffer = buffer;
  result.size = buffer_size;
  result.type = kScratchMemoryBufferInplace;
  return result;
}

void ScratchMemory_Free(ScratchMemoryBuffer* buffer) {
  if (buffer->type == kScratchMemoryBufferUnmanaged) {
    scratch_used_unmanaged_memory -= buffer->size;

    assert(scratch_used_unmanaged_memory >= 0 &&
           "Shouldn't delete more than allocated of unmanaged memory.");
  } else if (buffer->type == kScratchMemoryBufferManaged) {
    scratch_used_managed_memory += buffer->size;

    assert(scratch_used_managed_memory >= 0 &&
           "Shouldn't delete more than allocated of managed memory.");

    if (buffer->buffer) {
      free(buffer->buffer);
    }
  }

  buffer->buffer = 0;
  buffer->size = 0;
}

size_t ScratchMemory_UsedManagedMemory(void) {
  return scratch_used_managed_memory;
}

size_t ScratchMemory_UsedUnmanagedMemory(void) {
  return scratch_used_unmanaged_memory;
}
