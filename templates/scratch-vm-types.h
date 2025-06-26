#ifndef __SCRATCH_VM_INCLUDE_TYPES_H__
#define __SCRATCH_VM_INCLUDE_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef double ScratchNumber;

typedef struct ScratchVector2d {
  ScratchNumber x;
  ScratchNumber y;
} ScratchVector2d;

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_TYPES_H__
