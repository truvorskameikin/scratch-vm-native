#ifndef __SCRATCH_VM_INCLUDE_BLOCK_H__
#define __SCRATCH_VM_INCLUDE_BLOCK_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ScratchOpCode {
  kScratchWhenFlagClicked = 1,
  kScratchInPlace = 2,
  kScratchControlForever = 3,
  kScratchControlWait = 4,
  kScratchControlIf = 5,
} ScratchOpCode;

typedef enum ScratchBlockFunctionResult {
  kScratchBlockFunctionResultWait = 1,
  kScratchBlockFunctionResultDoneGoToSubstack = 2,
  kScratchBlockFunctionResultDoneGoToSubstackAlt = 3,
  kScratchBlockFunctionResultDone = 4,
} ScratchBlockFunctionResult;

typedef void (*ImplaceBlockFunction)(ScratchNumber dt);
typedef ScratchBlockFunctionResult (*BlockFunction)(void* runtime,
                                                    ScratchNumber dt);

typedef struct ScratchBlock {
  struct ScratchBlock* next;
  struct ScratchBlock* substack;
  struct ScratchBlock* substack_alt;
  ScratchOpCode op_code;
  union {
    ImplaceBlockFunction inplace_function;
    BlockFunction block_function;
  };
} ScratchBlock;

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_BLOCK_H__
