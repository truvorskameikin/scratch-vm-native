#include <gtest/gtest.h>

#include "templates/scratch-vm-program.h"

// =====
// Stack test.
// =====
typedef struct Runtime1 {
  char s[16];
} Runtime1;

typedef struct StackEntry1 {
  ScratchStackEntry _;
  Runtime1 runtime;
} StackEntry1;

typedef struct Runtime2 {
  int i;
} Runtime2;

typedef struct StackEntry2 {
  ScratchStackEntry _;
  Runtime2 runtime;
} StackEntry2;

typedef struct Runtime3 {
  char c;
  int i;
  double d;
} Runtime3;

typedef struct StackEntry3 {
  ScratchStackEntry _;
  Runtime3 runtime;
} StackEntry3;

typedef struct Runtime4 {
  char s[8];
} Runtime4;

typedef struct StackEntry4 {
  ScratchStackEntry _;
  Runtime4 runtime;
} StackEntry4;

TEST(scratch_vm_program_gtest, stack_basic) {
  const int num_runtimes = 4;
  size_t runtimes_sizes[num_runtimes] = {sizeof(Runtime1), sizeof(Runtime2),
                                         sizeof(Runtime3), sizeof(Runtime4)};

  const size_t buffer_size = sizeof(StackEntry1) + sizeof(StackEntry2) +
                             sizeof(StackEntry3) + sizeof(StackEntry4);
  char* buffer[buffer_size];

  ScratchMemoryBuffer runtimes_sizes_buffer =
      ScratchMemory_Alloc(runtimes_sizes, num_runtimes);
  ScratchMemoryBuffer stack_buffer = ScratchMemory_Alloc(buffer, buffer_size);
  ScratchStack stack = ScratchStack_Init(&stack_buffer, &runtimes_sizes_buffer);

  ScratchStackEntry* top = 0;

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top, nullptr);

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime1));
  Runtime1* r1 = (Runtime1*)top->runtime_buffer;
  strcpy(r1->s, "runtime1");

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime2));
  Runtime2* r2 = (Runtime2*)top->runtime_buffer;
  r2->i = 10;

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime3));
  Runtime3* r3 = (Runtime3*)top->runtime_buffer;
  r3->c = 'A';
  r3->i = 25;
  r3->d = 12345.5;

  ScratchStack_Pop(&stack);

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime2));
  r2 = (Runtime2*)top->runtime_buffer;
  ASSERT_EQ(r2->i, 10);

  ScratchStack_Pop(&stack);

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime1));
  r1 = (Runtime1*)top->runtime_buffer;
  ASSERT_EQ(std::string(r1->s), "runtime1");

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime2));
  r2 = (Runtime2*)top->runtime_buffer;
  r2->i = 20;

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime3));
  r3 = (Runtime3*)top->runtime_buffer;
  r3->c = 'B';
  r3->i = 50;
  r3->d = 54321.5;

  top = ScratchStack_Push(&stack, /*block=*/0);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime4));
  Runtime4* r4 = (Runtime4*)top->runtime_buffer;
  strcpy(r4->s, "1234567");

  ScratchStack_Pop(&stack);

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime3));
  r3 = (Runtime3*)top->runtime_buffer;
  ASSERT_EQ(r3->c, 'B');
  ASSERT_EQ(r3->i, 50);
  ASSERT_EQ(r3->d, 54321.5);

  ScratchStack_Pop(&stack);

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime2));
  r2 = (Runtime2*)top->runtime_buffer;
  ASSERT_EQ(r2->i, 20);

  ScratchStack_Pop(&stack);

  top = ScratchStack_GetTop(&stack);
  ASSERT_EQ(top->runtime_buffer_size, sizeof(Runtime1));
  r1 = (Runtime1*)top->runtime_buffer;
  ASSERT_EQ(std::string(r1->s), "runtime1");
}

// ====
// Program1 test.
// ====
typedef union Program1Runtime1 {
  ScratchRuntimeBase flag_clicked1_runtime;
  ScratchRuntimeBase inplace1_runtime;
  ScratchRuntimeControlWait wait1_runtime;
  ScratchRuntimeBase inplace2_runtime;
} Program1Runtime1;

typedef struct Program1StackEntry1 {
  ScratchStackEntry _;
  Program1Runtime1 runtime;
} Program1StackEntry1;

int program1_step = 0;

ScratchBlockFunctionResult Program1FlagClicked1Func(void* runtime,
                                                    ScratchNumber dt) {
  (void)dt;
  (void)runtime;
  program1_step = 1;
  return kScratchBlockFunctionResultDone;
}

void Program1Inplace1Func(ScratchNumber dt) {
  (void)dt;
  program1_step = 2;
}

ScratchBlockFunctionResult Program1Wait11Func(void* runtime, ScratchNumber dt) {
  ScratchRuntimeBase* runtime_base = (ScratchRuntimeBase*)runtime;
  ScratchRuntimeControlWait* runtime_wait = (ScratchRuntimeControlWait*)runtime;

  if (runtime_base->last_update_iteration == 0) {
    runtime_wait->timeout = 1;
  }

  return ScratchProgram_AdvanceControlWait(runtime_wait, dt);
}

void Program1Inplace2Func(ScratchNumber dt) {
  (void)dt;
  program1_step = 3;
}

TEST(scratch_vm_program_gtest, program1) {
  const int num_runtimes = 1;
  size_t runtimes_sizes[num_runtimes] = {sizeof(Program1Runtime1)};

  const size_t buffer_size = sizeof(Program1StackEntry1);
  char* buffer[buffer_size];

  ScratchMemoryBuffer runtimes_sizes_buffer =
      ScratchMemory_Alloc(runtimes_sizes, num_runtimes);
  ScratchMemoryBuffer stack_buffer = ScratchMemory_Alloc(buffer, buffer_size);
  ScratchProgram program =
      ScratchProgram_Init(&stack_buffer, &runtimes_sizes_buffer);

  ScratchBlock flag_clicked1;
  ScratchBlock inplace1;
  ScratchBlock wait1;
  ScratchBlock inplace2;

  flag_clicked1.op_code = kScratchWhenFlagClicked;
  flag_clicked1.block_function = Program1FlagClicked1Func;
  flag_clicked1.next = &inplace1;

  inplace1.op_code = kScratchInPlace;
  inplace1.inplace_function = Program1Inplace1Func;
  inplace1.next = &wait1;

  wait1.op_code = kScratchControlWait;
  wait1.block_function = Program1Wait11Func;
  wait1.next = &inplace2;

  inplace2.op_code = kScratchInPlace;
  inplace2.inplace_function = Program1Inplace2Func;
  inplace2.next = 0;

  ScratchProgram_Start(&program, &flag_clicked1);
  ASSERT_EQ(program1_step, 0);
  ScratchProgram_Advance(&program, 0.1);
  ASSERT_EQ(program1_step, 2);
  ScratchProgram_Advance(&program, 0.8);
  ASSERT_EQ(program1_step, 2);
  ScratchProgram_Advance(&program, 0.2);
  ASSERT_EQ(program1_step, 3);
}