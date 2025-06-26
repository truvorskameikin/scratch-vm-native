#ifndef __SCRATCH_VM_INCLUDE_MEMORY_H__
#define __SCRATCH_VM_INCLUDE_MEMORY_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ScratchMemoryBufferType {
  kScratchMemoryBufferManaged = 1,  // Alocated by Scratch VM.
  kScratchMemoryBufferUnmanaged =
      2,  // Alocated by something else or in most cases from data segment.
  kScratchMemoryBufferInplace =
      3,  // Pointer to memory already allocated and counted in managed or in
          // unmanaged categories.
} ScratchMemoryBufferType;

typedef struct ScratchMemoryBuffer {
  ScratchMemoryBufferType type;
  void* buffer;
  size_t size;
} ScratchMemoryBuffer;

ScratchMemoryBuffer ScratchMemory_Alloc(void* unmanaged_buffer,
                                        size_t buffer_size);
ScratchMemoryBuffer ScratchMemory_AllocInplace(void* buffer,
                                               size_t buffer_size);
void ScratchMemory_Free(ScratchMemoryBuffer* buffer);
size_t ScratchMemory_UsedManagedMemory(void);
size_t ScratchMemory_UsedUnmanagedMemory(void);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_MEMORY_H__
