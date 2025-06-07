#include <gtest/gtest.h>

#include "templates/scratch-vm-variables-internal.h"

TEST(scratch_vm_variables_gtest, strings_init) {
  ScratchVariable s1;
  Scratch_InitVariable(&s1);

  ScratchVariable s2;
  Scratch_InitStringVariable(&s2, "chicken", /*is_const_str_value=*/ 1);

  ScratchVariable s3;
  Scratch_InitStringVariable(&s3, "banana", /*is_const_str_value=*/ 1);

  ScratchVariable s4;
  Scratch_InitVariable(&s4);
  Scratch_AssignVariable(&s4, &s3);

  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s1)), "");
  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s2)), "chicken");
  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s3)), "banana");
  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s4)), "banana");

  Scratch_AssignStringVariable(&s1, " ");
  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s1)), " ");

  ScratchVariable s5 = Scratch_JoinStringVariables(&s2, &s1);
  ScratchVariable s6 = Scratch_JoinStringVariables(&s5, &s3);
  ASSERT_EQ(std::string(Scratch_ReadStringVariable(&s6)), "chicken banana");

  Scratch_FreeVariable(&s1);
  Scratch_FreeVariable(&s2);
  Scratch_FreeVariable(&s3);
  Scratch_FreeVariable(&s4);
  Scratch_FreeVariable(&s5);
  Scratch_FreeVariable(&s6);
}
