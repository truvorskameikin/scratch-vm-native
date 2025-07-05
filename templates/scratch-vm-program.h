#ifndef __SCRATCH_VM_INCLUDE_PROGRAM_H__
#define __SCRATCH_VM_INCLUDE_PROGRAM_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-block.h"
#include "scratch-vm-buffered-linked-list.h"
#include "scratch-vm-memory.h"
#include "scratch-vm-sprite.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchStackEntry {
  struct ScratchStackEntry* prev_entry;
  ScratchBlock* block;
  void* runtime_buffer;
  size_t runtime_buffer_size;
} ScratchStackEntry;

typedef struct ScratchStack {
  ScratchMemoryBuffer stack_buffer;
  ScratchMemoryBuffer runtimes_sizes_buffer;
  size_t num_runtimes;
  int cur_stack_index;
  ScratchStackEntry* cur_top;
} ScratchStack;

// All runtimes should inherit from ScratchRuntimeBase.
typedef struct ScratchRuntimeBase {
  unsigned int last_update_iteration;
  ScratchBlockFunctionResult last_iteration_result;
} ScratchRuntimeBase;

typedef struct ScratchRuntimeControlWait {
  ScratchRuntimeBase _;
  ScratchNumber current_wait_time;
  ScratchNumber timeout;
} ScratchRuntimeControlWait;

typedef struct ScratchProgram {
  unsigned int cur_iteration;
  ScratchStack stack;
} ScratchProgram;

size_t ScratchStack_MinBufferSize(ScratchMemoryBuffer* runtimes_sizes_buffer);
ScratchStack ScratchStack_Init(ScratchMemoryBuffer* stack_buffer,
                               ScratchMemoryBuffer* runtimes_sizes_buffer);
ScratchStackEntry* ScratchStack_Push(ScratchStack* stack, ScratchBlock* block);
ScratchStackEntry* ScratchStack_GetTop(ScratchStack* stack);
void ScratchStack_Pop(ScratchStack* stack);
int ScratchStack_Size(ScratchStack* stack);

ScratchProgram ScratchProgram_Init(ScratchMemoryBuffer* stack_buffer,
                                   ScratchMemoryBuffer* runtimes_sizes_buffer);
void ScratchProgram_Start(ScratchProgram* program, ScratchBlock* block);
void ScratchProgram_Advance(ScratchProgram* program, ScratchNumber dt);
ScratchBlockFunctionResult ScratchProgram_AdvanceControlWait(
    ScratchRuntimeControlWait* runtime, ScratchNumber dt);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_PROGRAM_H__
