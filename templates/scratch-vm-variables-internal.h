#ifndef SCRATCH_VM_INCLUDE_VARIABLES_INTERNAL_H_
#define SCRATCH_VM_INCLUDE_VARIABLES_INTERNAL_H_

#include "scratch-vm-types.h"
#include "scratch-vm-variables-public.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void Scratch_InitVariable(ScratchVariable* variable);
extern void Scratch_InitNumberVariable(ScratchVariable* variable, ScratchNumber number_value);
extern void Scratch_InitStringVariable(ScratchVariable* variable, const char* str, int is_const_str_value);

extern void Scratch_AssignNumberVariable(ScratchVariable* variable, ScratchNumber number);
extern void Scratch_AssignStringVariable(ScratchVariable* variable, const char* str);
extern void Scratch_AssignVariable(ScratchVariable* variable, ScratchVariable* rhv);

extern ScratchNumber Scratch_ReadNumberVariable(ScratchVariable* variable);
extern const char* Scratch_ReadStringVariable(ScratchVariable* variable);

extern ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1, ScratchVariable* variable2);

extern void Scratch_FreeVariable(ScratchVariable* variable);

#ifdef __cplusplus
}
#endif

#endif // #ifndef SCRATCH_VM_INCLUDE_VARIABLES_H_
