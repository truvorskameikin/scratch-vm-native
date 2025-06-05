#include "variables.h"

#include <gtest/gtest.h>

TEST(variables_gtest, simple) {
    Scratch_Init();

    Scratch_Advance(0.5);
    ScratchVariable* v1 = Scratch_FindVariable("Stage", "Number");
    ASSERT_FLOAT_EQ(v1->number_value, 7);
    ScratchVariable* v2 = Scratch_FindVariable("Stage", "Another Number");
    ASSERT_FLOAT_EQ(v2->number_value, 30);
    ScratchVariable* v3 = Scratch_FindVariable("Stage", "Text");
    ASSERT_EQ(std::string(v3->str_value), "Text");
    ScratchVariable* v4 = Scratch_FindVariable("Stage", "Another Text");
    ASSERT_EQ(std::string(v4->str_value), "Another Text");

    Scratch_Advance(0.4);
    ASSERT_EQ(std::string(v3->str_value), "Text");
    ASSERT_EQ(std::string(v4->str_value), "Another Text");

    Scratch_Advance(0.2);
    ASSERT_EQ(std::string(v3->str_value), "chicken banana chicken banana chicken banana banana banana banana");
    ASSERT_EQ(std::string(v4->str_value), "chicken");
}
