// All includes should be below these defines
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#if defined(SCRATCH_VM_ALLOW_INCLUDES)
#include "scratch-vm-types.h"
#include "scratch-vm-variables-public.h"
#include "scratch-vm-variables-internal.h"

#include <stdlib.h>
#include <string.h>
#endif

void Scratch_InitVariable(ScratchVariable* variable) {
  variable->number_value = 0;

  variable->str_value = 0;
  variable->is_const_str_value = 0;
}

void Scratch_InitNumberVariable(ScratchVariable* variable, ScratchNumber number_value) {
  variable->number_value = number_value;

  variable->str_value = 0;
  variable->is_const_str_value = 0;
}

void Scratch_AssignNumberVariable(ScratchVariable* variable, ScratchNumber number) {
  variable->number_value = number;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free((void*)variable->str_value);
    }
    variable->str_value = 0;
    variable->is_const_str_value = 0;
  }
}

ScratchNumber Scratch_ReadNumberVariable(ScratchVariable* variable) {
  return variable->number_value;
}

const char* Scratch_ReadStringVariable(ScratchVariable* variable) {
  if (variable->str_value) {
    return variable->str_value;
  }
  return "";
}

void Scratch_InitStringVariable(ScratchVariable* variable, const char* str, int is_const_str_value) {
  variable->number_value = 0;

  if (is_const_str_value) {
    variable->str_value = str;
    variable->is_const_str_value = is_const_str_value;
  } else {
    variable->str_value = strdup(str);
    variable->is_const_str_value = 0;
  }
}

void Scratch_AssignStringVariable(ScratchVariable* variable, const char* str) {
  variable->number_value = 0;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free((void*)variable->str_value);
    }
    variable->str_value = 0;
  }

  variable->str_value = strdup(str);
  variable->is_const_str_value = 0;
}

ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1, ScratchVariable* variable2) {
  const char* s1 = Scratch_ReadStringVariable(variable1);
  const char* s2 = Scratch_ReadStringVariable(variable2);
  size_t size1 = strlen(s1);
  size_t size2 = strlen(s2);
  
  char* new_string = malloc(size1 + size2 + 1);
  new_string = strcpy(new_string, s1);
  new_string = strcat(new_string, s2);

  ScratchVariable result;
  Scratch_InitStringVariable(&result, new_string, /*is_const_str_value=*/ 0);
  
  return result;
}

void Scratch_AssignVariable(ScratchVariable* variable, ScratchVariable* rhv) {
  if (rhv->str_value) {
    Scratch_AssignStringVariable(variable, rhv->str_value);
  } else {
    Scratch_AssignNumberVariable(variable, rhv->number_value);
  }
}

void Scratch_FreeVariable(ScratchVariable* variable) {
  variable->number_value = 0;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free((void*)variable->str_value);
    }
    variable->str_value = 0;
    variable->is_const_str_value = 0;
  }
}
