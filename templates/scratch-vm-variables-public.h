#ifndef SCRATCH_VM_INCLUDE_VARIABLES_PUBLIC_H_
#define SCRATCH_VM_INCLUDE_VARIABLES_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchVariable {
  ScratchNumber number_value;
  const char* str_value;
  int is_const_str_value;
} ScratchVariable;

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name);

#ifdef __cplusplus
}
#endif

#endif // #ifndef SCRATCH_VM_INCLUDE_VARIABLES_PUBLIC_H_
