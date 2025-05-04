// =====
// Scratch Engine definitions
// =====

#include <stdlib.h>
#include <string.h>

#include "{{ main_header_file }}"

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
{%- for variable in variables %}
ScratchVariable {{ variable.sprite_name }}_{{ variable.name }};
{%- endfor %}

// =====
// Function for inplace blocks
// =====

// =====
// Init
// =====
void Scratch_Init() {
{%- for variable in variables %}
{%- if variable.is_string %}
  Scratch_AssignStringVariable(&{{ variable.sprite_name }}_{{ variable.name }}, {{ variable.value }});
{%- else %}
  Scratch_AssignNumberVariable(&{{ variable.sprite_name }}_{{ variable.name }}, {{ variable.value }});
{%- endif %}
{%- endfor %}
}
