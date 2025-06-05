#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ScratchNumber double

void Scratch_Init(void);

typedef struct ScratchVariable {
  ScratchNumber number_value;
  char* str_value;
  int is_const_str_value;
} ScratchVariable;

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name);

void Scratch_Advance(ScratchNumber dt);

#ifdef __cplusplus
}
#endif

