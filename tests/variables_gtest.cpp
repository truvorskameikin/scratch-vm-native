#include "variables.h"

#include <gtest/gtest.h>

// TODO(truvorskameikin): Remove direct access to functions and variables. Tests should use Scratch_Advance and Scratch_FindVariable functions.
extern "C" {
struct ScratchSprite;
extern void Sprite1_Inplace1_function(ScratchSprite* sprite, float dt);
extern void Sprite1_Inplace3_function(ScratchSprite* sprite, float dt);
}

TEST(variables_gtest, simple) {
    Scratch_Init();

    Sprite1_Inplace1_function(0, 0.0f);
    ScratchVariable* v1 = Scratch_FindVariable("Stage", "Number");
    ASSERT_FLOAT_EQ(v1->number_value, 7);
    ScratchVariable* v2 = Scratch_FindVariable("Stage", "Another Number");
    ASSERT_FLOAT_EQ(v2->number_value, 30);

    Sprite1_Inplace3_function(0, 0.0f);
    ScratchVariable* v3 = Scratch_FindVariable("Stage", "Text");
    ASSERT_EQ(std::string(v3->str_value), "chicken banana chicken banana chicken banana banana banana banana");
    ScratchVariable* v4 = Scratch_FindVariable("Stage", "Another Text");
    ASSERT_EQ(std::string(v4->str_value), "chicken");
}
