// =====
// Scratch Engine definitions
// =====

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
void {{ block.block_name }}_function(ScratchSprite* sprite, float dt) {
{%- for scratch_block in block.scratch_inplace_blocks %}
{%- if block.scratch_inplace_blocks_helpers[loop.index0] != None %}
{%- if block.scratch_inplace_blocks[loop.index0]["opcode"] == "data_setvariableto" %}
  Scratch_AssignNumberVariable(&{{ block.scratch_inplace_blocks_helpers[loop.index0]["variable_name"] }}, {{ block.scratch_inplace_blocks_helpers[loop.index0]["value"] }});
{%- endif %}
{%- else %}
  // Produce code for op_code = {{ scratch_block["opcode"] }}
{%- endif %}
{%- endfor %}
}
{%- endif %}
{%- endfor %}

// =====
// Scratch state and functions
// =====
float current_time = 0.0f;

float Scratch_sensing_timer(struct ScratchSprite* sprite, float dt) {
  return current_time;
}

// =====
// Init
// =====
void Scratch_Init() {
{%- for variable in variables %}
{%- if variable.is_string %}
  Scratch_AssignStringVariable(&{{ variable.variable_name }}, {{ variable.value }});
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
  return 0;
}
