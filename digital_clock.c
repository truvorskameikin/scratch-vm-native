// =====
// Scratch Engine definitions
// =====

#include <stdlib.h>
#include <string.h>

#include "digital_clock.h"

struct ScratchSprite;

typedef enum ScratchOpCode {
kScratchWhenFlagClicked = 1,
kScratchInPlace = 2,
kScratchControlForever = 3,
kScratchControlIf = 4,
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
ScratchVariable Stage_my_variable;
ScratchVariable Stage_Minutes;
ScratchVariable Stage_Seconds;
ScratchVariable Stage_Prev_Time;
ScratchVariable Stage_Cur_Time;

// =====
// Blocks
// =====
ScratchBlock Stage_event_whenflagclicked0;
ScratchBlock Stage_Inplace1;
ScratchBlock Stage_control_forever2;
ScratchBlock Stage_Inplace3;
ScratchBlock Stage_control_if4;
ScratchBlock Stage_Inplace5;
ScratchBlock Stage_Inplace6;

// =====
// Inplace blocks functions
// =====
void Stage_Inplace1_function(ScratchSprite* sprite, float dt) {
  // Produce code for op_code = data_setvariableto
  // Produce code for op_code = data_setvariableto
}
void Stage_Inplace3_function(ScratchSprite* sprite, float dt) {
  // Produce code for op_code = data_setvariableto
}
void Stage_Inplace5_function(ScratchSprite* sprite, float dt) {
  // Produce code for op_code = data_setvariableto
  // Produce code for op_code = data_setvariableto
}
void Stage_Inplace6_function(ScratchSprite* sprite, float dt) {
  // Produce code for op_code = data_setvariableto
}

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
  Scratch_AssignNumberVariable(&Stage_my_variable, 0.0);
  Scratch_AssignNumberVariable(&Stage_Minutes, 4.0);
  Scratch_AssignNumberVariable(&Stage_Seconds, 1.0);
  Scratch_AssignNumberVariable(&Stage_Prev_Time, 20.096);
  Scratch_AssignNumberVariable(&Stage_Cur_Time, 21.845);
  Stage_event_whenflagclicked0.op_code = kScratchWhenFlagClicked;
  Stage_event_whenflagclicked0.next = &Stage_Inplace1;
  Stage_event_whenflagclicked0.substack = 0;
  Stage_event_whenflagclicked0.inplace_function = 0;
  Stage_Inplace1.op_code = kScratchInPlace;
  Stage_Inplace1.next = &Stage_control_forever2;
  Stage_Inplace1.substack = 0;
  Stage_Inplace1.inplace_function = Stage_Inplace1_function;
  Stage_control_forever2.op_code = kScratchControlForever;
  Stage_control_forever2.next = 0;
  Stage_control_forever2.substack = &Stage_Inplace3;
  Stage_control_forever2.inplace_function = 0;
  Stage_Inplace3.op_code = kScratchInPlace;
  Stage_Inplace3.next = &Stage_control_if4;
  Stage_Inplace3.substack = 0;
  Stage_Inplace3.inplace_function = Stage_Inplace3_function;
  Stage_control_if4.op_code = kScratchControlIf;
  Stage_control_if4.next = &Stage_Inplace6;
  Stage_control_if4.substack = &Stage_Inplace5;
  Stage_control_if4.inplace_function = 0;
  Stage_Inplace5.op_code = kScratchInPlace;
  Stage_Inplace5.next = 0;
  Stage_Inplace5.substack = 0;
  Stage_Inplace5.inplace_function = Stage_Inplace5_function;
  Stage_Inplace6.op_code = kScratchInPlace;
  Stage_Inplace6.next = 0;
  Stage_Inplace6.substack = 0;
  Stage_Inplace6.inplace_function = Stage_Inplace6_function;
}

ScratchVariable* Scratch_FindVariable(const char* sprite_name, const char* variable_name) {
  return 0;
}