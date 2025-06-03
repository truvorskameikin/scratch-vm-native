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

void Scratch_InitVariable(ScratchVariable* variable) {
  variable->number_value = 0;

  variable->str_value = 0;
  variable->is_const_str_value = 0;
}

void Scratch_InitNumberVariable(ScratchVariable* variable, float number_value) {
  variable->number_value = number_value;
  variable->str_value = 0;
}

void Scratch_AssignNumberVariable(ScratchVariable* variable, float number) {
  variable->number_value = number;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free(variable->str_value);
    }
    variable->str_value = 0;
  }
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

void Scratch_InitStringVariable(ScratchVariable* variable, char* str, int is_const_str_value) {
  variable->number_value = 0;
  variable->str_value = str;
  variable->is_const_str_value = is_const_str_value;
}

void Scratch_AssignStringVariable(ScratchVariable* variable, char* str) {
  variable->number_value = 0;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free(variable->str_value);
    }
    variable->str_value = 0;
  }

  variable->str_value = strdup(str);

  variable->is_const_str_value = 0;
}

ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1, ScratchVariable* variable2) {
  char* s1 = Scratch_ReadStringVariable(variable1);
  char* s2 = Scratch_ReadStringVariable(variable2);
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
      free(variable->str_value);
    }
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
  Scratch_InitNumberVariable(&result, {{ helper.arguments[0] }});
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_value_string" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitStringVariable(&result, "{{ helper.arguments[0] }}", /*is_const_str_value=*/ 1);
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_variable" %}
ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitVariable(&result);
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
  Scratch_InitNumberVariable(&result, Scratch_ReadNumberVariable(&num1) + Scratch_ReadNumberVariable(&num2));
  
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
  Scratch_InitNumberVariable(&result, Scratch_ReadNumberVariable(&num1) * Scratch_ReadNumberVariable(&num2));
  
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
  Scratch_InitNumberVariable(&result, Scratch_ReadNumberVariable(&num1) / Scratch_ReadNumberVariable(&num2));
  
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
  Scratch_InitNumberVariable(&result, sqrt(Scratch_ReadNumberVariable(&num)));

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
  Scratch_InitVariable(&{{ variable.variable_name }});
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
{%- for variable in variables %}
  if (strcmp("{{ variable.scratch_target_name }}", sprite_name) == 0 && strcmp("{{ variable.scratch_variable_name }}", variable_name) == 0) {
    return &{{ variable.variable_name }};
  }
{%- endfor %}
  return 0;
}

