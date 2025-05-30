// =====
// Scratch Engine definitions
// =====

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "{{ main_header_file }}"

struct ScratchSprite;

typedef enum ScratchOpCode {
kScratchWhenFlagClicked = 1,
kScratchInPlace = 2,
kScratchControlForever = 3,
kScratchControlIf = 4,
kScratchControlWait = 5,
} ScratchOpCode;

typedef void (*ImplaceBlockFunction)(struct ScratchSprite* sprite, float dt);

typedef struct ScratchBlock {
  struct ScratchBlock* next;
  struct ScratchBlock* substack;
  ScratchOpCode op_code;
  ImplaceBlockFunction inplace_function;
} ScratchBlock;

typedef struct ScratchSprite {
  float x;
  float y;
  float direction_x;
  float direction_y;
} ScratchSprite;

void Scratch_AssignNumberVariable(ScratchVariable* variable, float number) {
  if (variable->str_value) {
    free(variable->str_value);
    variable->str_value = 0;
  }

  variable->number_value = number;
}

double Scratch_ReadNumberVariable(ScratchVariable* variable) {
  return variable->number_value;
}

char* Scratch_ReadStringVariable(ScratchVariable* variable) {
  if (variable->str_value) {
    return variable->str_value;
  }
  return "";
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

ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1, ScratchVariable* variable2) {
  ScratchVariable result;

  char* s1 = Scratch_ReadStringVariable(variable1);
  char* s2 = Scratch_ReadStringVariable(variable2);
  size_t size1 = strlen(s1);
  size_t size2 = strlen(s2);
  
  char* new_string = malloc(size1 + size2 + 1);
  strcpy(s1, new_string);
  strcpy(s2, new_string + size1);

  Scratch_AssignStringVariable(&result, new_string);

  free(new_string);
  
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
  if (variable->str_value) {
    free(variable->str_value);
    variable->str_value = 0;
  }
}

// =====
// Variables
// =====
{%- for variable in variables %}
ScratchVariable {{ variable.variable_name }};
{%- endfor %}

// =====
// Blocks
// =====
{%- for block in blocks %}
ScratchBlock {{ block.block_name }};
{%- endfor %}

// =====
// Inplace blocks functions
// =====
{%- for block in blocks %}
{%- if block.op_code == "kScratchInPlace" %}
{%- for helpers in block.scratch_inplace_blocks_helpers %}
// Inplace block helper:
{%- for helper in helpers %}
{%- if helper.op_code == "read_value_number" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_AssignNumberVariable(&result, {{ helper.arguments[0] }});
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_value_string" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_AssignStringVariable(&result, "{{ helper.arguments[0] }}");
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_variable" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_AssignVariable(&result, &{{ helper.arguments[0] }});
  return result;
}
{%- endif %}
{%- if helper.op_code == "set_variable" %}
void {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num = {{ helper.arguments[0] }}(sprite, dt);
  Scratch_AssignVariable(&{{ helper.arguments[1] }}, &num);

  Scratch_FreeVariable(&num);
}
{%- endif %}
{%- if helper.op_code == "operator_add" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num1 = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable num2 = {{ helper.arguments[1] }}(sprite, dt);
  ScratchVariable result;
  Scratch_AssignNumberVariable(&result, Scratch_ReadNumberVariable(&num1) + Scratch_ReadNumberVariable(&num2));
  
  Scratch_FreeVariable(&num1);
  Scratch_FreeVariable(&num2);
  
  return result;
}
{%- endif %}
{%- if helper.op_code == "operator_multiply" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num1 = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable num2 = {{ helper.arguments[1] }}(sprite, dt);
  ScratchVariable result;
  Scratch_AssignNumberVariable(&result, Scratch_ReadNumberVariable(&num1) * Scratch_ReadNumberVariable(&num2));
  
  Scratch_FreeVariable(&num1);
  Scratch_FreeVariable(&num2);
  
  return result;
}
{%- endif %}
{%- if helper.op_code == "operator_divide" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num1 = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable num2 = {{ helper.arguments[1] }}(sprite, dt);
  ScratchVariable result;
  Scratch_AssignNumberVariable(&result, Scratch_ReadNumberVariable(&num1) / Scratch_ReadNumberVariable(&num2));
  
  Scratch_FreeVariable(&num1);
  Scratch_FreeVariable(&num2);
  
  return result;
}
{%- endif %}
{%- if helper.op_code == "operator_join" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num1 = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable num2 = {{ helper.arguments[1] }}(sprite, dt);
  ScratchVariable result = Scratch_JoinStringVariables(&num1, &num2);

  Scratch_FreeVariable(&num1);
  Scratch_FreeVariable(&num2);

  return result;
}
{%- endif %}
{%- if helper.op_code == "sqrt" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable result;
  Scratch_AssignNumberVariable(&result, Scratch_ReadNumberVariable(&num));

  Scratch_FreeVariable(&num);

  return result;
}
{%- endif %}
{%- endfor %}
{%- endfor %}
// Inplace block function:
void {{ block.block_name }}_function(ScratchSprite* sprite, float dt) {
{%- for function in block.scratch_functions %}
  {{ function }}(sprite, dt);
{%- endfor %}
}
{% endif %}
{%- endfor %}
// =====
// Scratch state and functions
// =====
float current_time = 0.0f;

float Scratch_sensing_timer(struct ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  return current_time;
}

// =====
// Init
// =====
void Scratch_Init(void) {
{%- for variable in variables %}
{%- if variable.is_string %}
  Scratch_AssignStringVariable(&{{ variable.variable_name }}, "{{ variable.value }}");
{%- else %}
  Scratch_AssignNumberVariable(&{{ variable.variable_name }}, {{ variable.value }});
{%- endif %}
{%- endfor %}

{%- for block in blocks %}
  {{ block.block_name }}.op_code = {{ block.op_code }};
{%- if block.next_block_name %}
  {{ block.block_name }}.next = &{{ block.next_block_name }};
{%- else %}
  {{ block.block_name }}.next = 0;
{%- endif %}
{%- if block.substack_block_name %}
  {{ block.block_name }}.substack = &{{ block.substack_block_name }};
{%- else %}
  {{ block.block_name }}.substack = 0;
{%- endif %}
{%- if block.op_code == "kScratchInPlace" %}
  {{ block.block_name }}.inplace_function = {{ block.block_name }}_function;
{%- else %}
  {{ block.block_name }}.inplace_function = 0;
{%- endif %}
{%- endfor %}
}

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name) {
  (void) sprite_name;
  (void) variable_name;
  return 0;
}

