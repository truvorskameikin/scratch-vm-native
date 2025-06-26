#include <gtest/gtest.h>

#include "templates/scratch-vm-buffered-linked-list.h"

typedef struct TestNode {
  ScratchBufferedListNode _;
  int value;
} TestNode;

TEST(scratch_vm_buffered_linked_list_gtest, basic_no_init_buffer) {
  ScratchBufferedList list = ScratchBufferedList_Init(
      sizeof(TestNode), /*num_initial_nodes=*/4, /*num_next_buffer_nodes=*/4,
      /*first_buffer=*/0);

  ASSERT_EQ(ScratchBufferedList_Capacity(&list), 4);

  TestNode* node1 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node1->value = 1;

  TestNode* node2 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node2->value = 2;

  TestNode* node3 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node3->value = 3;

  TestNode* node4 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node4->value = 4;

  TestNode* node5 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node5->value = 5;

  TestNode* node6 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node6->value = 6;

  TestNode* node7 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node7->value = 7;

  ASSERT_EQ(ScratchBufferedList_Capacity(&list), 8);

  ASSERT_EQ(ScratchBufferedList_Size(&list), 7);
  ASSERT_EQ(node1->value, 1);
  ASSERT_EQ(node2->value, 2);
  ASSERT_EQ(node3->value, 3);
  ASSERT_EQ(node4->value, 4);
  ASSERT_EQ(node5->value, 5);
  ASSERT_EQ(node6->value, 6);
  ASSERT_EQ(node7->value, 7);

  int i = 1;
  TestNode* cur_node = 0;
  for (cur_node = (TestNode*)ScratchBufferedList_GetFirst(&list);
       cur_node != nullptr; cur_node = (TestNode*)ScratchBufferedList_GetNext(
                                &list, (ScratchBufferedListNode*)cur_node)) {
    ASSERT_EQ(cur_node->value, i);
    ++i;
  }

  ASSERT_EQ(i, ScratchBufferedList_Size(&list) + 1);

  ScratchBufferedList_Remove(&list, (ScratchBufferedListNode*)node1);
  ScratchBufferedList_Remove(&list, (ScratchBufferedListNode*)node4);
  ScratchBufferedList_Remove(&list, (ScratchBufferedListNode*)node5);
  ScratchBufferedList_Remove(&list, (ScratchBufferedListNode*)node7);

  ASSERT_EQ(ScratchBufferedList_Size(&list), 3);
  cur_node = (TestNode*)ScratchBufferedList_GetFirst(&list);
  ASSERT_EQ(cur_node->value, 2);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 3);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 6);

  TestNode* node8 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node8->value = 8;

  TestNode* node9 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node9->value = 9;

  ASSERT_EQ(ScratchBufferedList_Capacity(&list), 8);

  ASSERT_EQ(ScratchBufferedList_Size(&list), 5);
  cur_node = (TestNode*)ScratchBufferedList_GetFirst(&list);
  ASSERT_EQ(cur_node->value, 2);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 3);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 6);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 8);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 9);

  TestNode* node10 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node10->value = 10;

  TestNode* node11 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node11->value = 11;

  TestNode* node12 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node12->value = 12;

  ASSERT_EQ(ScratchBufferedList_Size(&list), 8);
  ASSERT_EQ(ScratchBufferedList_Capacity(&list), 8);

  TestNode* node13 = (TestNode*)ScratchBufferedList_InsertNew(&list);
  node13->value = 13;

  ASSERT_EQ(ScratchBufferedList_Size(&list), 9);
  ASSERT_EQ(ScratchBufferedList_Capacity(&list), 12);

  cur_node = (TestNode*)ScratchBufferedList_GetFirst(&list);
  ASSERT_EQ(cur_node->value, 2);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 3);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 6);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 8);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 9);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 10);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 11);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 12);
  cur_node = (TestNode*)ScratchBufferedList_GetNext(
      &list, (ScratchBufferedListNode*)cur_node);
  ASSERT_EQ(cur_node->value, 13);
}
