#ifndef __SCRATCH_VM_INCLUDE_VARIABLE_TYPES_H__
#define __SCRATCH_VM_INCLUDE_VARIABLE_TYPES_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchVariable {
  ScratchNumber number_value;
  const char* str_value;
  int is_const_str_value;
} ScratchVariable;

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_VARIABLE_TYPES_H__
