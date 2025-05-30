#include "variables.h"

#include <gtest/gtest.h>

// TODO(truvorskameikin): Remove direct access to functions and variables. Tests should use Scratch_Advance and Scratch_FindVariable functions.
extern "C" {
struct ScratchSprite;
extern void Sprite1_Inplace1_function(ScratchSprite* sprite, float dt);
extern ScratchVariable Stage_Number;
extern ScratchVariable Stage_Another_Number;
}

TEST(variables_gtest, simple) {
    Scratch_Init();
    Sprite1_Inplace1_function(0, 0.0f);
    ASSERT_FLOAT_EQ(Stage_Number.number_value, 7);
    ASSERT_FLOAT_EQ(Stage_Another_Number.number_value, 30);
}
