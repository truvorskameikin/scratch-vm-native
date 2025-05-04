// =====
// Scratch Engine definitions
// =====

#include <stdlib.h>
#include <string.h>

#include "repeat_until_x.h"

struct ScratchVariable;

typedef struct ScratchVariable {
  float number_value;
  char* str_value;
} ScratchVariable;

void Scratch_AssignNumberVariable(ScratchVariable* variable, float number) {
  if (variable->str_value) {
    free(variable->str_value);
    variable->str_value = 0;
  }

  variable->number_value = number;
}

void Scratch_AssignStringVariable(ScratchVariable* variable, const char* str) {
  if (variable->str_value) {
    free(variable->str_value);
    variable->str_value = 0;
  }

  size_t l = strlen(str);
  variable->str_value = malloc(l + 1);
  memcpy(variable->str_value, str, l + 1);
}

// =====
// Variables
// =====
ScratchVariable Stage_my_variable;
ScratchVariable red_100x70_x;
ScratchVariable red_100x70_y;

// =====
// Function for inplace blocks
// =====

// =====
// Init
// =====
void Scratch_Init() {
  Scratch_AssignNumberVariable(&Stage_my_variable, 0.0);
  Scratch_AssignNumberVariable(&red_100x70_x, 51.0);
  Scratch_AssignNumberVariable(&red_100x70_y, 0.0);
}