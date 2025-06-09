// All includes should be below these defines
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#include <math.h>
#include <stdlib.h>
#include <string.h>

{% include 'scratch-vm-types.h' with context %}

{% include 'scratch-vm-variables-public.h' with context %}

{% include 'scratch-vm-variables.c' with context %}

{% set sprite_base %}
  ScratchNumber x;
  ScratchNumber y;
  ScratchNumber direction_x;
  ScratchNumber direction_y;
{%- endset %}

typedef struct ScratchSprite {
{{ sprite_base }}
} ScratchSprite;

typedef enum ScratchOpCode {
kScratchWhenFlagClicked = 1,
kScratchInPlace = 2,
kScratchControlForever = 3,
kScratchControlIf = 4,
kScratchControlWait = 5,
} ScratchOpCode;

typedef enum ScratchBlockFunctionResult {
kScratchBlockFunctionResultContinue = 1,
kScratchBlockFunctionResultWait = 2,
} ScratchBlockFunctionResult;

typedef void (*ImplaceBlockFunction)(ScratchNumber dt);
typedef ScratchBlockFunctionResult (*BlockFunction)(ScratchNumber dt);
typedef ScratchVariable (*ExpressionFunction)(ScratchSprite* sprite, ScratchNumber dt);

typedef struct ScratchBlock {
  struct ScratchBlock* next;
  struct ScratchBlock* substack;
  ScratchOpCode op_code;
  union {
    ImplaceBlockFunction inplace_function;
    BlockFunction block_function;
  };
} ScratchBlock;

typedef struct ScratchControlWaitRuntime {
  int is_running;
  ScratchNumber currentWaitTime;
  ScratchNumber timeout;
} ScratchControlWaitRuntime;

ScratchBlockFunctionResult Scratch_AdvanceControlWaitRuntime(
    ScratchNumber dt,
    ScratchSprite* sprite,
    ExpressionFunction duration_expression,
    ScratchControlWaitRuntime* runtime) {
  
  if (!runtime->is_running) {
    ScratchVariable duration = duration_expression(sprite, dt);
    runtime->timeout = Scratch_ReadNumberVariable(&duration);
    Scratch_FreeVariable(&duration);

    runtime->is_running = 1;
  }

  runtime->currentWaitTime += dt;
  if (runtime->currentWaitTime > runtime->timeout) {
    runtime->currentWaitTime = runtime->timeout;
  }
  if (runtime->currentWaitTime == runtime->timeout) {
    runtime->is_running = 0;
    return kScratchBlockFunctionResultContinue;
  }
  return kScratchBlockFunctionResultWait;
}

void Scratch_AdvanceSingleProgram(ScratchNumber dt, ScratchBlock* stack[], int* cur_stack_index, int is_in_sub_stack) {
  if (*cur_stack_index < 0) {
    return;
  }

  if (stack[*cur_stack_index] == 0) {
    if (is_in_sub_stack) {
      return;
    }

    --(*cur_stack_index);
  }

  if (*cur_stack_index < 0) {
    return;
  }

  ScratchBlock* cur_block = stack[*cur_stack_index];
  if (cur_block->op_code == kScratchInPlace) {
    cur_block->inplace_function(dt);
  } else {
    ScratchBlockFunctionResult result = cur_block->block_function(dt);
    if (result == kScratchBlockFunctionResultWait) {
      return;
    }
  }

  stack[*cur_stack_index] = stack[*cur_stack_index]->next;
  Scratch_AdvanceSingleProgram(dt, stack, cur_stack_index, is_in_sub_stack);
}

// =====
// Targets
// =====
{% for target in targets %}
typedef struct {{ target.clone_c_struct_name }} {
{{ sprite_base }}
  struct {{ target.clone_c_struct_name }}* next;
} {{ target.clone_c_struct_name }};
typedef struct {{ target.c_struct_name }} {
{{ sprite_base }}
  {{ target.clone_c_struct_name }}* clones;
} {{ target.c_struct_name }};
{{ target.c_struct_name }} {{ target.variable_name }};
{% endfor %}

// =====
// Variables
// =====
{% for variable in variables %}
static ScratchVariable {{ variable.variable_name }};
{% endfor %}

// =====
// Blocks
// =====
{% for block in blocks %}
static ScratchBlock {{ block.block_name }};
{% endfor %}

// =====
// kScratchWhenFlagClicked programs
// =====
{% for block in when_flag_clicked_blocks %}
typedef struct {
  int is_running;
  ScratchBlock* stack[{{ block.max_level + 1 }}];
  int cur_stack_index;
  int is_in_sub_stack;
} {{ block.block_name }}_program_t;
{{ block.block_name }}_program_t {{ block.block_name }}_program;
void Scratch_Advance_{{ block.block_name }}_program(ScratchNumber dt) {
  Scratch_AdvanceSingleProgram(dt, {{ block.block_name }}_program.stack, &{{ block.block_name }}_program.cur_stack_index, {{ block.block_name }}_program.is_in_sub_stack);
}
{% endfor %}

// =====
// Inplace blocks functions
// =====
{% for block in blocks %}
{% for helpers in block.scratch_input_helpers %}
// Inplace block helper:
{% for helper in helpers %}
{% if helper.op_code == "read_value_number" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitNumberVariable(&result, {{ helper.arguments[0] }});
  return result;
}
{% endif %}
{% if helper.op_code == "read_value_string" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitStringVariable(&result, "{{ helper.arguments[0] }}", /*is_const_str_value=*/ 1);
  return result;
}
{% endif %}
{% if helper.op_code == "read_variable" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable result;
  Scratch_InitVariable(&result);
  Scratch_AssignVariable(&result, &{{ helper.arguments[0] }});
  return result;
}
{% endif %}
{% if helper.op_code == "set_variable" %}
static inline void {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num = {{ helper.arguments[0] }}(sprite, dt);
  Scratch_AssignVariable(&{{ helper.arguments[1] }}, &num);

  Scratch_FreeVariable(&num);
}
{% endif %}
{% if helper.op_code == "operator_add" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
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
{% endif %}
{% if helper.op_code == "operator_multiply" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
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
{% endif %}
{% if helper.op_code == "operator_divide" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
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
{% endif %}
{% if helper.op_code == "operator_join" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num1 = {{ helper.arguments[0] }}(sprite, dt);
  ScratchVariable num2 = {{ helper.arguments[1] }}(sprite, dt);
  
  ScratchVariable result = Scratch_JoinStringVariables(&num1, &num2);

  Scratch_FreeVariable(&num1);
  Scratch_FreeVariable(&num2);

  return result;
}
{% endif %}
{% if helper.op_code == "sqrt" %}
static inline ScratchVariable {{ helper.function_name }}(ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  ScratchVariable num = {{ helper.arguments[0] }}(sprite, dt);
  
  ScratchVariable result;
  Scratch_InitNumberVariable(&result, sqrt(Scratch_ReadNumberVariable(&num)));

  Scratch_FreeVariable(&num);

  return result;
}
{% endif %}
{% endfor %}
{% endfor %}
{% if block.op_code == "kScratchInPlace" %}
// Will not be inlined because pointer to this function will be used as inline block function.
void {{ block.block_name }}_function(ScratchNumber dt) {
{% for function in block.scratch_functions %}
  {{ function }}((ScratchSprite*) &{{ block.target.variable_name }}, dt);
{% endfor %}
}
{% elif block.op_code == "kScratchControlWait" %}
// TODO(truvorskameikin): Move runtime block to target and clone.
ScratchControlWaitRuntime {{ block.block_name }}_runtime;
ScratchBlockFunctionResult {{ block.block_name }}_function(ScratchNumber dt) {
  return Scratch_AdvanceControlWaitRuntime(
      dt,
      (ScratchSprite*) &{{ block.target.variable_name }},
      {{ block.scratch_input_helpers[0][-1].function_name }},
      &{{ block.block_name }}_runtime);
}
{% else %}
ScratchBlockFunctionResult {{ block.block_name }}_function(ScratchNumber dt) {
  (void) dt;
  return kScratchBlockFunctionResultContinue;
}
{% endif %}
{% endfor %}

// =====
// Scratch state and functions
// =====
static ScratchNumber current_time = 0.0f;

static inline ScratchNumber Scratch_sensing_timer(struct ScratchSprite* sprite, ScratchNumber dt) {
  (void) sprite;
  (void) dt;
  return current_time;
}

// =====
// Init
// =====
void Scratch_Init(void) {
  // Variables
{% for variable in variables %}
{% if variable.is_string %}
  Scratch_InitStringVariable(&{{ variable.variable_name }}, "{{ variable.value }}", /*is_const_str_value=*/ 1);
{% else %}
  Scratch_InitNumberVariable(&{{ variable.variable_name }}, {{ variable.value }});
{% endif %}
{% endfor %}

  // Targets
{% for target in targets %}
  {{target.variable_name}}.x = 0;
  {{target.variable_name}}.y = 0;
  {{target.variable_name}}.direction_x = 0;
  {{target.variable_name}}.direction_y = 0;
  {{target.variable_name}}.clones = 0;
{% endfor %}

  // Blocks
{% for block in blocks %}
  {{ block.block_name }}.op_code = {{ block.op_code }};
{% if block.next_block_name %}
  {{ block.block_name }}.next = &{{ block.next_block_name }};
{% else %}
  {{ block.block_name }}.next = 0;
{% endif %}
{% if block.substack_block_name %}
  {{ block.block_name }}.substack = &{{ block.substack_block_name }};
{% else %}
  {{ block.block_name }}.substack = 0;
{% endif %}
{% if block.op_code == "kScratchInPlace" %}
  {{ block.block_name }}.inplace_function = {{ block.block_name }}_function;
{% else %}
{% if block.op_code == "kScratchControlWait" %}
  {{ block.block_name }}_runtime.is_running = 0;
  {{ block.block_name }}_runtime.currentWaitTime = 0;
  {{ block.block_name }}_runtime.timeout = 0;
{% endif %}
  {{ block.block_name }}.block_function = {{ block.block_name }}_function;
{% endif %}
{% endfor %}

{% for block in when_flag_clicked_blocks %}
  {{ block.block_name }}_program.stack[0] = &{{ block.block_name }};
  {{ block.block_name }}_program.cur_stack_index = 0;
{% endfor %}
}

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name) {
{% for variable in variables %}
  if (strcmp("{{ variable.scratch_target_name }}", sprite_name) == 0 && strcmp("{{ variable.scratch_variable_name }}", variable_name) == 0) {
    return &{{ variable.variable_name }};
  }
{% endfor %}
  return 0;
}

void Scratch_Advance(ScratchNumber dt) {
{% for block in when_flag_clicked_blocks %}
  Scratch_Advance_{{ block.block_name }}_program(dt);
{% endfor %}
}

// Need two new lines in the end.

