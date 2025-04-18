#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

typedef union Param
{
  int i;
  uint64_t u64;
  double d;
} Param;

struct TargetState;
typedef struct Block
{
  int (*update)(struct TargetState *state, int dtMs);
  struct Block *next;
  int isActive;
  Param param0;
  Param param1;
  Param param2;

  Param state0;
  Param state1;
  Param state2;
  Param state3;
  Param state4;
} Block;

typedef struct Target
{
  const char *name;
  const Block *blocks;
} Target;

typedef struct TargetState
{
  Target *target;
  int x;
  int y;
  Block *currentBlock;
} TargetState;

int BlockEventWhenFlagClicked(TargetState *state, int dtMs);
int BlockMotionGlideSecsToXY(TargetState *state, int dtMs);
int BlockControlWait(TargetState *state, int dtMs);

static Block block3 = {
    .update = BlockMotionGlideSecsToXY,
    .next = NULL,
    .param0.i = 0,
    .param1.i = 0,
    .param2.u64 = 2000,
};

static Block block2 = {
    .update = BlockControlWait,
    .next = &block3,
    .param0.u64 = 500,
};

static Block block1 = {
    .update = BlockMotionGlideSecsToXY,
    .next = &block2,
    .param0.i = 100,
    .param1.i = 100,
    .param2.u64 = 1000,
};

static Block block0 = {
    .update = BlockEventWhenFlagClicked,
    .next = &block1,
};

static Target target0 = {
    .name = "target1",
    .blocks =
        &block0,
};

static uint64_t globalTime = 0;

int main()
{
  TargetState state = {
      .target = &target0,
      .x = 0,
      .y = 0,
      .currentBlock = target0.blocks,
  };

  const int dtMs = 1000 / 30;
  while (1)
  {
    Block *currentBlock = state.currentBlock;

    printf("[t: %zu]\n", globalTime);

    int ret = currentBlock->update(&state, dtMs);
    if (ret == 0)
    {
      if (state.currentBlock->next != NULL)
      {
        // next block
        state.currentBlock = state.currentBlock->next;
      }
      else
      {
        // restart
        state.currentBlock = state.target->blocks;
        // stop yet
        exit(2);
      }
      state.currentBlock->isActive = 0;
      continue;
    }

    if (ret < 0)
    {
      printf("ERROR: %d\n", ret);
      exit(ret);
    }

    globalTime += dtMs;
  }
}

int BlockEventWhenFlagClicked(TargetState *state, int dtMs)
{
  printf("%s\n", __func__);
  // move to next block on the next tick
  return 0;
}

int BlockMotionGlideSecsToXY(TargetState *state, int dtMs)
{
  Block *currentBlock = state->currentBlock;
  uint64_t timeMs = currentBlock->param2.u64;
  if (!currentBlock->isActive)
  {
    // vx = (x - x0) / secs
    double vx = (currentBlock->param0.i - state->x) * 1.0 / timeMs;

    // vy = (y - y0) / secs
    double vy = (currentBlock->param1.i - state->y) * 1.0 / timeMs;

    currentBlock->state0.d = vx;
    currentBlock->state1.d = vy;
    currentBlock->state2.u64 = globalTime;
    currentBlock->state3.i = state->x;
    currentBlock->state4.i = state->y;

    currentBlock->isActive = 1;
  }
  int isLast = 0;
  int dt = globalTime - currentBlock->state2.u64;
  if (dt > timeMs)
  {
    isLast = 1;
    dt = timeMs;
  }

  state->x = currentBlock->state3.i + dt * currentBlock->state0.d;
  state->y = currentBlock->state4.i + dt * currentBlock->state1.d;

  printf("%s[%zu]: x:%d, y:%d\n", __func__, globalTime, state->x, state->y);

  if (isLast)
  {
    // Done, next block
    return 0;
  }

  // continue execution
  return 1;
}

int BlockControlWait(TargetState *state, int dtMs)
{
  Block *currentBlock = state->currentBlock;
  if (!currentBlock->isActive)
  {
    currentBlock->state0.u64 = (globalTime + currentBlock->param0.u64);
    currentBlock->isActive = 1;
  }
  if (currentBlock->state0.u64 > globalTime)
  {
    // continue execution
    printf("%s[%zu]: wait for %zu\n", __func__, globalTime, currentBlock->state0.u64 - globalTime);
    return 1;
  }

  // Move to next block
  return 0;
}
