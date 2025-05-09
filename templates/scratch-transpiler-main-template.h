// Scratch Engine definitions

extern void Scratch_Init();

typedef struct ScratchVariable {
  float number_value;
  char* str_value;
} ScratchVariable;

extern ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name);
