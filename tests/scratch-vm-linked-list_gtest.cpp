#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "templates/scratch-vm-linked-list.h"

using ::testing::ElementsAre;

typedef struct TestNode {
  ScratchListNode _;
  int value;
} TestNode;

std::vector<int> toVector(ScratchList* list) {
  std::vector<int> result;
  for (ScratchListNode* cur_node = ScratchList_GetFirst(list); cur_node != 0;
       cur_node = ScratchList_GetNext(list, cur_node)) {
    TestNode* test_node = (TestNode*)cur_node;
    result.push_back(test_node->value);
  }

  return result;
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_1) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertFront(&list, (ScratchListNode*)&node1);
  ScratchList_InsertFront(&list, (ScratchListNode*)&node2);
  ScratchList_InsertBack(&list, (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(2, 1, 3));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_2) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertBack(&list, (ScratchListNode*)&node1);
  ScratchList_InsertBack(&list, (ScratchListNode*)&node2);
  ScratchList_InsertFront(&list, (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(3, 1, 2));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_3) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertBack(&list, (ScratchListNode*)&node1);
  ScratchList_InsertBack(&list, (ScratchListNode*)&node2);
  ScratchList_InsertBack(&list, (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(1, 2, 3));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_4) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertFront(&list, (ScratchListNode*)&node1);
  ScratchList_InsertFront(&list, (ScratchListNode*)&node2);
  ScratchList_InsertFront(&list, (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(3, 2, 1));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_after_1) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertFront(&list, (ScratchListNode*)&node1);
  ScratchList_InsertAfter(&list, (ScratchListNode*)&node1,
                          (ScratchListNode*)&node2);
  ScratchList_InsertAfter(&list, (ScratchListNode*)&node1,
                          (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(1, 3, 2));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_after_2) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertBack(&list, (ScratchListNode*)&node1);
  ScratchList_InsertAfter(&list, (ScratchListNode*)&node1,
                          (ScratchListNode*)&node2);
  ScratchList_InsertAfter(&list, (ScratchListNode*)&node2,
                          (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(1, 2, 3));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_before_1) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertFront(&list, (ScratchListNode*)&node1);
  ScratchList_InsertBefore(&list, (ScratchListNode*)&node1,
                           (ScratchListNode*)&node2);
  ScratchList_InsertBefore(&list, (ScratchListNode*)&node1,
                           (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(2, 3, 1));
}

TEST(scratch_vm_linked_list_gtest, unsorted_inserts_before_2) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertFront(&list, (ScratchListNode*)&node1);
  ScratchList_InsertBefore(&list, (ScratchListNode*)&node1,
                           (ScratchListNode*)&node2);
  ScratchList_InsertBefore(&list, (ScratchListNode*)&node2,
                           (ScratchListNode*)&node3);
  ASSERT_EQ(ScratchList_GetSize(&list), 3);
  ASSERT_THAT(toVector(&list), ElementsAre(3, 2, 1));
}

int comp(ScratchListNode* lhv, ScratchListNode* rhv) {
  TestNode* test_node_l = (TestNode*)lhv;
  TestNode* test_node_r = (TestNode*)rhv;
  if (test_node_l->value < test_node_r->value) {
    return -1;
  } else if (test_node_l->value > test_node_r->value) {
    return 1;
  }
  return 0;
}

TEST(scratch_vm_linked_list_gtest, sorted_inserts) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  TestNode node4;
  node4.value = 4;

  TestNode node5;
  node5.value = 5;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node3, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node2, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node5, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node1, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node4, comp);
  ASSERT_EQ(ScratchList_GetSize(&list), 5);
  ASSERT_THAT(toVector(&list), ElementsAre(1, 2, 3, 4, 5));
}

TEST(scratch_vm_linked_list_gtest, remove) {
  TestNode node1;
  node1.value = 1;

  TestNode node2;
  node2.value = 2;

  TestNode node3;
  node3.value = 3;

  TestNode node4;
  node4.value = 4;

  TestNode node5;
  node5.value = 5;

  ScratchList list = ScratchList_Init();
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node3, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node2, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node5, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node1, comp);
  ScratchList_InsertSorted(&list, (ScratchListNode*)&node4, comp);

  ScratchList_Remove(&list, (ScratchListNode*)&node1);
  ScratchList_Remove(&list, (ScratchListNode*)&node5);
  ScratchList_Remove(&list, (ScratchListNode*)&node3);

  ASSERT_EQ(ScratchList_GetSize(&list), 2);
  ASSERT_THAT(toVector(&list), ElementsAre(2, 4));
}
