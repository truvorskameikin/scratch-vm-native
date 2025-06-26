#pragma once

#define __SCRATCH_VM_INSIDE_TEMPLATE__

{% include 'scratch-vm-types.h' without context %}

{% include 'scratch-vm-variable-types.h' without context %}

{% include 'scratch-vm-sprite-types.h' without context %}

#ifdef __cplusplus
extern "C" {
#endif

void Scratch_Init(void);
void Scratch_Advance(ScratchNumber dt);
void Scratch_AdvanceSteps(ScratchNumber dt, ScratchNumber fps);

ScratchVariable* Scratch_FindVariable(const char* sprite_name,
                                      const char* variable_name);

#ifdef __cplusplus
}
#endif

// Need two new lines in the end.

