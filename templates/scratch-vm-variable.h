#ifndef __SCRATCH_VM_INCLUDE_VARIABLE_H__
#define __SCRATCH_VM_INCLUDE_VARIABLE_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-variable-types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Scratch_InitVariable(ScratchVariable* variable);
void Scratch_InitNumberVariable(ScratchVariable* variable,
                                ScratchNumber number_value);
void Scratch_InitStringVariable(ScratchVariable* variable, const char* str,
                                int is_const_str_value);

void Scratch_AssignNumberVariable(ScratchVariable* variable,
                                  ScratchNumber number);
void Scratch_AssignStringVariable(ScratchVariable* variable, const char* str);
void Scratch_AssignVariable(ScratchVariable* variable, ScratchVariable* rhv);

ScratchNumber Scratch_ReadNumberVariable(ScratchVariable* variable);
const char* Scratch_ReadStringVariable(ScratchVariable* variable);

ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1,
                                            ScratchVariable* variable2);

void Scratch_FreeVariable(ScratchVariable* variable);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_VARIABLE_H__
