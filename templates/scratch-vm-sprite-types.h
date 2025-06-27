#ifndef __SCRATCH_VM_INCLUDE_SPRITE_TYPES_H__
#define __SCRATCH_VM_INCLUDE_SPRITE_TYPES_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchSprite {
  ScratchVector2d position;
  ScratchNumber direction;
  ScratchNumber size;
  int costume_number;
  int is_clone;
  void* internal_pointer_1;
} ScratchSprite;

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_SPRITE_TYPES_H__
