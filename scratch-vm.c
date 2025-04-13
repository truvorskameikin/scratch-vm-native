#include <stdio.h>
#include <assert.h>

int kOpCodeControlRepeat = 100;
int kOpCodeControlWait = 101;
int kOpCodeMotionMoveSteps = 200;

typedef struct Sprite {
  int x;
  int y;
} Sprite;

typedef struct BlockHeader {
  struct Sprite* sprite;
  int op_code_type;
  struct BlockHeader* next;
} BlockHeader;

typedef struct BlockControlRepeat {
  struct BlockHeader header;
  int times;
  int iterations;
  struct BlockHeader* sub_stack;
} BlockControlRepeat;

typedef struct BlockControlWait {
  BlockHeader header;
  int wait_millis;
  int cur_wait;
} BlockControlWait;

typedef struct BlockMotionMoveSteps {
  struct BlockHeader header;
  int steps;
} BlockMotionMoveSteps;

void Advance(int* stack_index, BlockHeader* stack[], int dt_millis, int is_in_substack) {
  if (*stack_index < 0) {
    return;
  }

  if (stack[*stack_index] == 0) {
    if (is_in_substack) {
      return;
    }

    --(*stack_index);
  }

  if (*stack_index < 0) {
    return;
  }

  if (stack[*stack_index]->op_code_type == kOpCodeControlRepeat) {
    BlockControlRepeat* repeat = (BlockControlRepeat*) stack[*stack_index];
    // printf("kOpCodeControlRepeat, times: %i\n", repeat->times);
    if (repeat->iterations < repeat->times) {
      ++(*stack_index);
      stack[*stack_index] = repeat->sub_stack;

      ++repeat->iterations;

      Advance(stack_index, stack, dt_millis, 1);
      return;
    }
  }

  if (stack[*stack_index]->op_code_type == kOpCodeControlWait) {
    BlockControlWait* wait = (BlockControlWait*) stack[*stack_index];
    // printf("kOpCodeControlWait, wait_millis: %i\n", wait->wait_millis);
    wait->cur_wait += dt_millis;
    if (wait->cur_wait >= wait->wait_millis) {
      wait->cur_wait = wait->wait_millis;
    }
    if (wait->cur_wait < wait->wait_millis) {
      return;
    }
  }

  if (stack[*stack_index]->op_code_type == kOpCodeMotionMoveSteps) {
    BlockMotionMoveSteps* move = (BlockMotionMoveSteps*) stack[*stack_index];
    // printf("kOpCodeMotionMoveSteps, steps: %i\n", move->steps);
    stack[*stack_index]->sprite->x += move->steps;
  }

  stack[*stack_index] = stack[*stack_index]->next;
  Advance(stack_index, stack, dt_millis, is_in_substack);
}

int main(int argc, char* argv[]) {
  Sprite sprite;
  sprite.x = 0;
  sprite.y = 0;

  BlockMotionMoveSteps move1;
  BlockMotionMoveSteps move2;
  BlockMotionMoveSteps move3;
  BlockMotionMoveSteps move4;

  BlockControlRepeat repeat;
  repeat.header.sprite = &sprite;
  repeat.header.op_code_type = kOpCodeControlRepeat;
  repeat.times = 4;
  repeat.iterations = 0;

  BlockControlWait wait;
  wait.header.sprite = &sprite;
  wait.header.op_code_type = kOpCodeControlWait;
  wait.wait_millis = 1000;

  move1.header.sprite = &sprite;
  move1.header.op_code_type = kOpCodeMotionMoveSteps;
  move1.header.next = &move2.header;
  move1.steps = 1;
  move2.header.sprite = &sprite;
  move2.header.op_code_type = kOpCodeMotionMoveSteps;
  move2.header.next = &wait.header;
  move2.steps = 1;

  wait.header.next = &move3.header;
  wait.cur_wait = 0;

  move3.header.sprite = &sprite;
  move3.header.op_code_type = kOpCodeMotionMoveSteps;
  move3.header.next = &move4.header;
  move3.steps = 1;
  move4.header.sprite = &sprite;
  move4.header.op_code_type = kOpCodeMotionMoveSteps;
  move4.header.next = &repeat.header;
  move4.steps = 1;

  BlockMotionMoveSteps move5;
  BlockMotionMoveSteps move6;
  BlockMotionMoveSteps move7;
  BlockMotionMoveSteps move8;

  move5.header.sprite = &sprite;
  move5.header.op_code_type = kOpCodeMotionMoveSteps;
  move5.header.next = &move6.header;
  move5.steps = 2;
  move6.header.sprite = &sprite;
  move6.header.op_code_type = kOpCodeMotionMoveSteps;
  move6.header.next = &move7.header;
  move6.steps = 2;
  move7.header.sprite = &sprite;
  move7.header.op_code_type = kOpCodeMotionMoveSteps;
  move7.header.next = &move8.header;
  move7.steps = 2;
  move8.header.sprite = &sprite;
  move8.header.op_code_type = kOpCodeMotionMoveSteps;
  move8.header.next = 0;
  move8.steps = 2;

  repeat.sub_stack = &move5.header;

  BlockHeader* stack[5];
  stack[0] = &move1.header;
  int stack_index = 0;
  int cur_time = 0;
  int dt_millis = 0;

  printf("Advance, cur_time: %i\n", cur_time);
  dt_millis = 16;
  Advance(&stack_index, stack, dt_millis, 0);
  cur_time += dt_millis;
  printf("sprite.x: %i\n", sprite.x);
  assert(sprite.x == 2);

  printf("Advance, cur_time: %i\n", cur_time);
  dt_millis = 84;
  Advance(&stack_index, stack, dt_millis, 0);
  cur_time += dt_millis;
  printf("sprite.x: %i\n", sprite.x);
  assert(sprite.x == 2);

  printf("Advance, cur_time: %i\n", cur_time);
  dt_millis = 910;
  Advance(&stack_index, stack, dt_millis, 0);
  cur_time += dt_millis;
  printf("sprite.x: %i\n", sprite.x);
  assert(sprite.x == 12);

  return 0;
}
