#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-program.h"

#include <assert.h>
#endif

// =====
// ScratchStack
// =====
size_t ScratchStack_MinBufferSize(ScratchMemoryBuffer* runtimes_sizes_buffer) {
  int num_runtimes = runtimes_sizes_buffer->size / sizeof(size_t);
  size_t* runtimes_sizes = runtimes_sizes_buffer->buffer;

  size_t result = 0;
  for (int i = 0; i < num_runtimes; ++i) {
    result += runtimes_sizes[i] + sizeof(ScratchStackEntry);
  }
  return result;
}

ScratchStack ScratchStack_Init(ScratchMemoryBuffer* stack_buffer,
                               ScratchMemoryBuffer* runtimes_sizes_buffer) {
  assert(ScratchStack_MinBufferSize(runtimes_sizes_buffer) <=
             stack_buffer->size &&
         "Stack buffer should be big enough to hold all runtimes.");

  ScratchStack result;
  result.stack_buffer = *stack_buffer;
  result.runtimes_sizes_buffer = *runtimes_sizes_buffer;
  result.num_runtimes = runtimes_sizes_buffer->size / sizeof(size_t);
  result.cur_stack_index = -1;
  result.cur_top = 0;
  return result;
}

ScratchStackEntry* ScratchStack_Push(ScratchStack* stack, ScratchBlock* block) {
  ScratchStackEntry* prev_top = 0;
  ScratchStackEntry* cur_top = 0;
  if (!stack->cur_top) {
    cur_top = (ScratchStackEntry*)stack->stack_buffer.buffer;
  } else {
    prev_top = stack->cur_top;

    cur_top = (ScratchStackEntry*)((char*)prev_top + sizeof(ScratchStackEntry) +
                                   prev_top->runtime_buffer_size);
  }

  ++stack->cur_stack_index;
  assert(stack->cur_stack_index < stack->num_runtimes &&
         "Pushing too much to stack.");

  size_t* runtimes_sizes = stack->runtimes_sizes_buffer.buffer;

  cur_top->prev_entry = prev_top;
  cur_top->block = block;
  cur_top->runtime_buffer_size = runtimes_sizes[stack->cur_stack_index];
  cur_top->runtime_buffer = (char*)cur_top + sizeof(ScratchStackEntry);

  stack->cur_top = cur_top;

  return stack->cur_top;
}

ScratchStackEntry* ScratchStack_GetTop(ScratchStack* stack) {
  return stack->cur_top;
}

void ScratchStack_Pop(ScratchStack* stack) {
  assert(stack->cur_top && "Pop called on empty stack");

  --stack->cur_stack_index;

  ScratchStackEntry* prev_entry = stack->cur_top->prev_entry;
  stack->cur_top = prev_entry;
}

int ScratchStack_Size(ScratchStack* stack) {
  return stack->cur_stack_index + 1;
}

// =====
// ScratchProgram
// =====
static int ScratchProgram_IsBlockDone(ScratchBlockFunctionResult result) {
  if (result == kScratchBlockFunctionResultDone ||
      result == kScratchBlockFunctionResultDoneGoToSubstack ||
      result == kScratchBlockFunctionResultDoneGoToSubstackAlt) {
    return 1;
  }
  return 0;
}

static void ScratchProgram_ToNextBlock(ScratchProgram* program,
                                       ScratchBlock* next_block) {
  ScratchStackEntry* cur_top = ScratchStack_GetTop(&program->stack);
  ScratchRuntimeBase* runtime_base =
      (ScratchRuntimeBase*)cur_top->runtime_buffer;

  cur_top->block = next_block;
  runtime_base->last_update_iteration = 0;
}

ScratchProgram ScratchProgram_Init(ScratchMemoryBuffer* stack_buffer,
                                   ScratchMemoryBuffer* runtimes_sizes_buffer) {
  ScratchProgram result;
  result.cur_iteration = 1;
  result.stack = ScratchStack_Init(stack_buffer, runtimes_sizes_buffer);
  return result;
}

void ScratchProgram_Start(ScratchProgram* program, ScratchBlock* block) {
  ScratchStackEntry* top = ScratchStack_Push(&program->stack, block);
  ScratchRuntimeBase* new_runtime_base =
      (ScratchRuntimeBase*)top->runtime_buffer;
  new_runtime_base->last_update_iteration = 0;
}

void ScratchProgram_Advance(ScratchProgram* program, ScratchNumber dt) {
  ++program->cur_iteration;

  while (1) {
    if (ScratchStack_Size(&program->stack) == 0) {
      break;
    }

    ScratchStackEntry* cur_top = ScratchStack_GetTop(&program->stack);
    if (cur_top->block == 0) {
      ScratchStack_Pop(&program->stack);
      continue;
    }

    cur_top = ScratchStack_GetTop(&program->stack);
    ScratchRuntimeBase* runtime_base =
        (ScratchRuntimeBase*)cur_top->runtime_buffer;

    if (runtime_base->last_update_iteration == program->cur_iteration) {
      if (ScratchProgram_IsBlockDone(runtime_base->last_iteration_result)) {
        ScratchProgram_ToNextBlock(program, cur_top->block->next);
        continue;
      } else {
        break;
      }
    } else {
      ScratchBlock* cur_block = cur_top->block;
      if (cur_block->op_code == kScratchInPlace) {
        cur_block->inplace_function(dt);

        ScratchProgram_ToNextBlock(program, cur_top->block->next);
        continue;
      } else {
        ScratchBlockFunctionResult result =
            cur_block->block_function(runtime_base, dt);
        runtime_base->last_update_iteration = program->cur_iteration;

        if (result == kScratchBlockFunctionResultWait) {
          break;
        } else if (result == kScratchBlockFunctionResultDone) {
          ScratchProgram_ToNextBlock(program, cur_top->block->next);
          continue;
        } else if (result == kScratchBlockFunctionResultDoneGoToSubstack ||
                   result == kScratchBlockFunctionResultDoneGoToSubstackAlt) {
          ScratchStackEntry* new_top =
              ScratchStack_Push(&program->stack, cur_block->substack);
          ScratchRuntimeBase* new_runtime_base =
              (ScratchRuntimeBase*)new_top->runtime_buffer;
          new_runtime_base->last_update_iteration = 0;
        } else {
          assert(0 && "Block function returned unexpected result.");
        }
      }
    }
  }
}

ScratchBlockFunctionResult ScratchProgram_AdvanceControlWait(
    ScratchRuntimeControlWait* runtime, ScratchNumber dt) {
  runtime->current_wait_time += dt;
  if (runtime->current_wait_time > runtime->timeout) {
    runtime->current_wait_time = runtime->timeout;
    return kScratchBlockFunctionResultDone;
  }

  return kScratchBlockFunctionResultWait;
}
