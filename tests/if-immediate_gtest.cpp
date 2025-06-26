#include <gtest/gtest.h>

#include "if-immediate.h"

TEST(if_immediate_gtest, simple) {
  Scratch_Init();

  ScratchVariable* a = Scratch_FindVariable("Stage", "A");
  ASSERT_TRUE(a != 0);
  ScratchVariable* b = Scratch_FindVariable("Stage", "B");
  ASSERT_TRUE(b != 0);

  Scratch_Advance(0.1);
  ASSERT_FLOAT_EQ(a->number_value, 9);
  ASSERT_FLOAT_EQ(b->number_value, 10);
}
