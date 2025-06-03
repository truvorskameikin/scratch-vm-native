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

static inline void Scratch_InitVariable(ScratchVariable* variable) {
  variable->number_value = 0;

  variable->str_value = 0;
  variable->is_const_str_value = 0;
}

static inline void Scratch_InitNumberVariable(ScratchVariable* variable, float number_value) {
  variable->number_value = number_value;
  variable->str_value = 0;
}

static inline void Scratch_AssignNumberVariable(ScratchVariable* variable, float number) {
  variable->number_value = number;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free(variable->str_value);
    }
    variable->str_value = 0;
  }
}

static inline double Scratch_ReadNumberVariable(ScratchVariable* variable) {
  return variable->number_value;
}

static inline char* Scratch_ReadStringVariable(ScratchVariable* variable) {
  if (variable->str_value) {
    return variable->str_value;
  }
  return "";
}

static inline void Scratch_InitStringVariable(ScratchVariable* variable, char* str, int is_const_str_value) {
  variable->number_value = 0;
  variable->str_value = str;
  variable->is_const_str_value = is_const_str_value;
}

static inline void Scratch_AssignStringVariable(ScratchVariable* variable, char* str) {
  variable->number_value = 0;

  if (variable->str_value) {
    if (!variable->is_const_str_value) {
      free(variable->str_value);
    }
    variable->str_value = 0;
  }

  size_t l = strlen(str);
  variable->str_value = malloc(l + 1);
  memcpy(variable->str_value, str, l + 1);

  variable->is_const_str_value = 0;
}

static inline ScratchVariable Scratch_JoinStringVariables(ScratchVariable* variable1, ScratchVariable* variable2) {
  char* s1 = Scratch_ReadStringVariable(variable1);
  char* s2 = Scratch_ReadStringVariable(variable2);
  size_t size1 = strlen(s1);
  size_t size2 = strlen(s2);
  
  char* new_string = malloc(size1 + size2 + 1);
  strcpy(new_string, s1);
  strcpy(new_string + size1, s2);

  ScratchVariable result;
  Scratch_InitStringVariable(&result, new_string, /*is_const_str_value=*/ 0);
  
  return result;
}

static inline void Scratch_AssignVariable(ScratchVariable* variable, ScratchVariable* rhv) {
  if (rhv->str_value) {
    Scratch_AssignStringVariable(variable, rhv->str_value);
  } else {
    Scratch_AssignNumberVariable(variable, rhv->number_value);
  }
}

static inline void Scratch_FreeVariable(ScratchVariable* variable) {
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
static ScratchVariable {{ variable.variable_name }};
{%- endfor %}

// =====
// Blocks
// =====
{%- for block in blocks %}
static ScratchBlock {{ block.block_name }};
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
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitNumberVariable(&result, {{ helper.arguments[0] }});
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_value_string" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitStringVariable(&result, "{{ helper.arguments[0] }}", /*is_const_str_value=*/ 1);
  return result;
}
{%- endif %}
{%- if helper.op_code == "read_variable" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitVariable(&result);
  Scratch_AssignVariable(&result, &{{ helper.arguments[0] }});
  return result;
}
{%- endif %}
{%- if helper.op_code == "set_variable" %}
static inline void {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num = {{ helper.arguments[0] }}(sprite, dt);
  Scratch_AssignVariable(&{{ helper.arguments[1] }}, &num);

  Scratch_FreeVariable(&num);
}
{%- endif %}
{%- if helper.op_code == "operator_add" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
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
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
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
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
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
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
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
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, float dt) {
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
// Make static inline after fixing test
// Inplace block function:
/*static inline*/ void {{ block.block_name }}_function(ScratchSprite* sprite, float dt) {
{%- for function in block.scratch_functions %}
  {{ function }}(sprite, dt);
{%- endfor %}
}
{% endif %}
{%- endfor %}
// =====
// Scratch state and functions
// =====
static float current_time = 0.0f;

static inline float Scratch_sensing_timer(struct ScratchSprite* sprite, float dt) {
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

