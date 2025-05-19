// Scratch Engine definitions
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void Scratch_Init();

typedef struct ScratchVariable {
  float number_value;
  char* str_value;
} ScratchVariable;

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name);

#ifdef __cplusplus
}
#endif