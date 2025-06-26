#include <gtest/gtest.h>

#include "digital-clock.h"

TEST(digital_clock_gtest, simple) {
  Scratch_Init();

  ScratchVariable* minutes = Scratch_FindVariable("Stage", "Minutes");
  ASSERT_TRUE(minutes != 0);
  ScratchVariable* seconds = Scratch_FindVariable("Stage", "Seconds");
  ASSERT_TRUE(seconds != 0);
  ScratchVariable* cur_time = Scratch_FindVariable("Stage", "Cur Time");
  ASSERT_TRUE(cur_time != 0);

  Scratch_Advance(0.4);
  ASSERT_FLOAT_EQ(minutes->number_value, 0);
  // This happens due to the fact that sensing_timer doesn't advance on its own.
  // And 'Cur Time' == 'Prev Time' at first iteration of forever loop which is
  // not the case in Scratch.
  ASSERT_FLOAT_EQ(seconds->number_value, -1);
  ASSERT_FLOAT_EQ(cur_time->number_value, 0.4);

  Scratch_Advance(0.1);
  ASSERT_FLOAT_EQ(minutes->number_value, 0);
  ASSERT_FLOAT_EQ(seconds->number_value, 0);

  Scratch_Advance(1.0);
  ASSERT_FLOAT_EQ(minutes->number_value, 0);
  ASSERT_FLOAT_EQ(seconds->number_value, 1);

  Scratch_Advance(1.0);
  ASSERT_FLOAT_EQ(minutes->number_value, 0);
  ASSERT_FLOAT_EQ(seconds->number_value, 2);

  Scratch_AdvanceSteps(/*dt=*/60.0, /*fps=*/30.0);
  ASSERT_FLOAT_EQ(minutes->number_value, 1);
  ASSERT_FLOAT_EQ(seconds->number_value, 2);
}
