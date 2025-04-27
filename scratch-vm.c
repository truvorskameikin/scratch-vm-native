#include <stdio.h>
#include <assert.h>

int kOpCodeRunInplace = 1;
int kOpCodeControlRepeat = 100;
int kOpCodeControlWait = 101;
int kOpCodeMotionMoveSteps = 200;

struct Variable;
struct Sprite;
struct BlockControlWait;

typedef float (*NumberExpressionFunction)(struct Sprite*);

typedef void (*BlockFunction)(struct Sprite*, float dt);

typedef int (*WaitFunction)(struct Sprite*, struct BlockControlWait* block, float dt);

typedef struct Variable {
  float number_value;
  char* str_value;
} Variable;

typedef struct BlockBase {
  BlockFunction function;
  int op_code;
  struct BlockBase* next;
} BlockBase;

typedef struct Program {
  int stack_index;
  BlockBase* stack[4];
  int is_running;
} Program;

typedef struct Sprite {
  float x;
  float y;
  float dir_x; // normalized
  float dir_y;
  // Variables:
  Variable step;
  Variable wait;
  // Programs:
  Program when_flag_clicked1;
  // Blocks state:
  int block_wait1_started;
  float block_wait1_wait_time;
  float block_wait1_cur_wait_time;
  int block_wait2_started;
  float block_wait2_wait_time;
  float block_wait2_cur_wait_time;
  int block_repeat1_iterations;
} Sprite;

typedef struct ClonedSprite {
  Sprite* parent;
} ClonedSprite;

void MakeRunInplaceBlock(BlockBase* block, Sprite* s, BlockFunction func) {
  block->function = func;
  block->op_code = kOpCodeRunInplace;
}

typedef struct BlockControlRepeat {
  struct BlockBase base;
  int times;
  struct BlockBase* sub_stack;
} BlockControlRepeat;

void MakeControlRepeatBlock(BlockControlRepeat* block, Sprite* s, int times) {
  block->base.function = 0;
  block->base.op_code = kOpCodeControlRepeat;
  block->times = times;
}

typedef void (*WaitBlockFunction)(Sprite*, float dt, int* can_advance);

typedef struct BlockControlWait {
  BlockBase base;
  NumberExpressionFunction wait_time;
  WaitFunction wait_function;
} BlockControlWait;

void MakeControlWaitBlock(BlockControlWait* block, NumberExpressionFunction wait_time, WaitFunction wait_function) {
  block->base.function = 0;
  block->base.op_code = kOpCodeControlWait;
  block->wait_time = wait_time;
  block->wait_function = wait_function;
}

void Advance(Sprite* sprite, Program* program, float dt_sec, int is_in_substack) {
  if (program->stack_index < 0) {
    return;
  }

  if (program->stack[program->stack_index] == 0) {
    if (is_in_substack) {
      return;
    }

    --(program->stack_index);
  }

  if (program->stack_index < 0) {
    return;
  }

  printf("Op code: %i, ", program->stack[program->stack_index]->op_code);

  if (program->stack[program->stack_index]->function) {
    printf("RunInplaceBlock\n");
    program->stack[program->stack_index]->function(sprite, dt_sec);
  } else if (program->stack[program->stack_index]->op_code == kOpCodeControlRepeat) {
    BlockControlRepeat* repeat = (BlockControlRepeat*) program->stack[program->stack_index];
  } else if (program->stack[program->stack_index]->op_code == kOpCodeControlWait) {
    printf("BlockControlWait\n");
    BlockControlWait* wait = (BlockControlWait*) program->stack[program->stack_index];
    int result = wait->wait_function(sprite, wait, dt_sec);
    if (!result) {
      return;
    }
  }

  program->stack[program->stack_index] = program->stack[program->stack_index]->next;
  Advance(sprite, program, dt_sec, is_in_substack);
}

// Program:
//
// Set 'step' 2
// Set 'wait' 1000 ms
// Move 1
// Move step
// Set step step + 1
// Wait 1000 ms
// Move 1
// Move step + 1
// Wait wait + 2
// Repeat 4:
//    Move 1
//    Move 1
//    Move 1
//    Move 1

// Results in:
//
// Block: RunInplace1:
//    Set step expression:
//        2
//    Set wait expression:
//        1000 ms
//    Move expression:
//        1
//    Move expression:
//        step
//    Set step expression:
//        step + 1
// Block: Wait expression:
//     1000 ms
// Block: RunInplace2:
//    Move expression:
//        1
//    Move expression:
//        step + 1
// Block: Wait expression:
//     wait + 2
// Block: Repeat expression:
//     4
//    Block: RunInplace3
//       Move expression:
//           1
//       Move expression:
//           1
//       Move expression:
//           1
//       Move expression:
//           1

void main_character_blockRunInplace1_func(Sprite* s, float dt) {
  // Set 'step' 2
  s->step.number_value = 2;

  // Set 'wait' 1000 ms
  s->wait.number_value = 1;

  // Move 1
  s->x += 1;

  // Move 1
  s->x += s->step.number_value;

  // Set 'step' as 'step' + 1
  s->step.number_value = s->step.number_value + 1;
}

float main_character_blockWait1_wait_time(Sprite* s) {
  return 1;
}

int main_character_blockWait1_wait_function(Sprite* s, struct BlockControlWait* block, float dt) {
  if (!s->block_wait1_started) {
    s->block_wait1_wait_time = block->wait_time(s);
    s->block_wait1_started = 1;
  }

  s->block_wait1_cur_wait_time += dt;
  if (s->block_wait1_cur_wait_time > s->block_wait1_wait_time) {
    s->block_wait1_cur_wait_time = s->block_wait1_wait_time;
    return 1;
  }
  return 0;
}

void main_character_blockRunInplace2_func(Sprite* s, float dt) {
  // Move 1
  s->x += 1;

  // Move 'step' + 1
  {
    float dx = s->step.number_value + 1;
    s->x += dx;
  }
}

float main_character_blockWait2_wait_time(Sprite* s) {
  return s->wait.number_value + 2;
}

int main_character_blockWait2_wait_function(Sprite* s, struct BlockControlWait* block, float dt) {
  if (!s->block_wait2_started) {
    s->block_wait2_wait_time = block->wait_time(s);
    s->block_wait2_started = 1;
  }

  s->block_wait2_cur_wait_time += dt;
  if (s->block_wait2_cur_wait_time > s->block_wait2_wait_time) {
    s->block_wait2_cur_wait_time = s->block_wait2_wait_time;
    return 1;
  }
  return 0;
}

void main_character_blockRunInplace3_func(Sprite* s, float dt) {
  // Move 1
  s->x += 1;

  // Move 1
  s->x += 1;

  // Move 1
  s->x += 1;

  // Move 1
  s->x += 1;
}

int main(int argc, char* argv[]) {
  Sprite main_character;
  main_character.x = 0;
  main_character.y = 0;

  // Variables:
  main_character.step.number_value = 0;
  main_character.step.str_value = 0;

  // Blocks state:
  main_character.block_wait1_started = 0;
  main_character.block_wait1_wait_time = 0;
  main_character.block_wait1_cur_wait_time = 0;
  main_character.block_wait2_started = 0;
  main_character.block_wait2_wait_time = 0;
  main_character.block_wait2_cur_wait_time = 0;
  main_character.block_repeat1_iterations = 0;

  BlockBase main_character_blockRunInplace1;
  BlockControlWait main_character_blockWait1;
  BlockBase main_character_blockRunInplace2;
  BlockControlWait main_character_blockWait2;
  BlockControlRepeat main_character_blockRepeat1;
  BlockBase main_character_blockRunInplace3;

  main_character.when_flag_clicked1.stack_index = 0;
  main_character.when_flag_clicked1.stack[0] = &main_character_blockRunInplace1;
  main_character.when_flag_clicked1.is_running = 0;

  MakeRunInplaceBlock(&main_character_blockRunInplace1, &main_character, main_character_blockRunInplace1_func);
  main_character_blockRunInplace1.next = &main_character_blockWait1.base;

  MakeControlWaitBlock(&main_character_blockWait1, main_character_blockWait1_wait_time, main_character_blockWait1_wait_function);
  main_character_blockWait1.base.next = &main_character_blockRunInplace2;

  MakeRunInplaceBlock(&main_character_blockRunInplace2, &main_character, main_character_blockRunInplace2_func);
  main_character_blockRunInplace2.next = &main_character_blockWait2.base;

  MakeControlWaitBlock(&main_character_blockWait2, main_character_blockWait2_wait_time, main_character_blockWait2_wait_function);
  main_character_blockWait2.base.next = &main_character_blockRepeat1.base;

  MakeControlRepeatBlock(&main_character_blockRepeat1, &main_character, 4);
  main_character_blockRepeat1.sub_stack = &main_character_blockRunInplace3;
  main_character_blockRepeat1.base.next = 0;
  main_character_blockRunInplace3.next = &main_character_blockRepeat1.base;

  MakeRunInplaceBlock(&main_character_blockRunInplace3, &main_character, main_character_blockRunInplace3_func);
  main_character_blockRunInplace3.next = 0;

  int cur_time = 0;
  int dt_millis = 0;

  dt_millis = 16;
  cur_time += dt_millis;
  printf("Advance, cur_time: %i\n", cur_time);
  Advance(&main_character, &main_character.when_flag_clicked1, dt_millis / 1000.0f, 0);
  printf(" sprite.x: %f\n", main_character.x);
  assert(main_character.x == 3);
  printf(" sprite.step.number_value: %f\n", main_character.step.number_value);
  printf(" sprite.wait.number_value: %f\n", main_character.wait.number_value);
  assert(main_character.step.number_value == 3);
  printf(" Ok\n");

  dt_millis = 84;
  cur_time += dt_millis;
  printf("Advance, cur_time: %i\n", cur_time);
  Advance(&main_character, &main_character.when_flag_clicked1, dt_millis / 1000.0f, 0);
  printf(" sprite.x: %f\n", main_character.x);
  assert(main_character.x == 3);
  printf(" Ok\n");

  dt_millis = 910;
  cur_time += dt_millis;
  printf("Advance, cur_time: %i\n", cur_time);
  Advance(&main_character, &main_character.when_flag_clicked1, dt_millis / 1000.0f, 0);
  printf(" sprite.x: %f\n", main_character.x);
  assert(main_character.x == 8);
  printf(" Ok\n");

  dt_millis = 1980;
  cur_time += dt_millis;
  printf("Advance, cur_time: %i\n", cur_time);
  Advance(&main_character, &main_character.when_flag_clicked1, dt_millis / 1000.0f, 0);
  printf(" sprite.x: %f\n", main_character.x);
  assert(main_character.x == 8);
  printf(" Ok\n");

  return 0;
}
