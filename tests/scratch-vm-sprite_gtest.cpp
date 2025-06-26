#include <gtest/gtest.h>

#include "templates/scratch-vm-sprite.h"

// TEST(scratch_vm_sprite_gtest, init) {
//   ScratchSprite sprite0;
//   sprite0.position = {0, 0};
//   sprite0.direction = 0;
//   sprite0.size = 0;
//   sprite0.costume_number = 0;

//   ScratchSprite sprite1;
//   sprite1.position = {1, 1};
//   sprite1.direction = 1;
//   sprite1.size = 1;
//   sprite1.costume_number = 1;

//   ScratchSprite sprite2;
//   sprite2.position = {2, 2};
//   sprite2.direction = 2;
//   sprite2.size = 2;
//   sprite2.costume_number = 2;

//   ScratchSprite sprite100;
//   sprite100.position = {100, 100};
//   sprite100.direction = 100;
//   sprite100.size = 100;
//   sprite100.costume_number = 100;

//   const size_t num_sprites = 4;
//   ScratchInternalSprite internal_sprites_raw_memory[num_sprites];
//   ScratchMemoryBuffer all_sprites_memory_buffer = ScratchMemory_Alloc(
//       internal_sprites_raw_memory, sizeof(ScratchInternalSprite) *
//       num_sprites);

//   ScratchAllSprites all_sprites =
//       ScratchAllSprites_Init(&all_sprites_memory_buffer, num_sprites);

//   ScratchAllSprites_InitSingleSprite(&all_sprites, 0, &sprite0);
//   ScratchAllSprites_InitSingleSprite(&all_sprites, 1, &sprite1);
//   ScratchAllSprites_InitSingleSprite(&all_sprites, 2, &sprite2);
//   ScratchAllSprites_InitSingleSprite(&all_sprites, 3, &sprite100);

//   ScratchSprite* s0 = ScratchAllSprites_GetFirst(&all_sprites);
//   ASSERT_EQ(s0->position.x, 0);
//   ASSERT_EQ(s0->position.y, 0);
//   ASSERT_EQ(s0->direction, 0);
//   ASSERT_EQ(s0->size, 0);
//   ASSERT_EQ(s0->costume_number, 0);

//   ScratchSprite* s1 = ScratchAllSprites_GetNext(&all_sprites, s0);
//   ASSERT_EQ(s1->position.x, 1);
//   ASSERT_EQ(s1->position.y, 1);
//   ASSERT_EQ(s1->direction, 1);
//   ASSERT_EQ(s1->size, 1);
//   ASSERT_EQ(s1->costume_number, 1);

//   ScratchSprite* s2 = ScratchAllSprites_GetNext(&all_sprites, s1);
//   ASSERT_EQ(s2->position.x, 2);
//   ASSERT_EQ(s2->position.y, 2);
//   ASSERT_EQ(s2->direction, 2);
//   ASSERT_EQ(s2->size, 2);
//   ASSERT_EQ(s2->costume_number, 2);

//   ScratchSprite* s3 = ScratchAllSprites_GetNext(&all_sprites, s2);
//   ASSERT_EQ(s3->position.x, 100);
//   ASSERT_EQ(s3->position.y, 100);
//   ASSERT_EQ(s3->direction, 100);
//   ASSERT_EQ(s3->size, 100);
//   ASSERT_EQ(s3->costume_number, 100);
// }

// TEST(scratch_vm_sprite_gtest, clones) {
//   ScratchSprite sprite0;
//   sprite0.position = {0, 0};
//   sprite0.direction = 0;
//   sprite0.size = 0;
//   sprite0.costume_number = 0;

//   ScratchSprite sprite1;
//   sprite1.position = {1, 1};
//   sprite1.direction = 1;
//   sprite1.size = 1;
//   sprite1.costume_number = SCRATCH_VM_NUM_PREALLOCATED_CLONES + 10;

//   const size_t num_sprites = 2;
//   ScratchInternalSprite internal_sprites_raw_memory[num_sprites];
//   ScratchMemoryBuffer all_sprites_memory_buffer = ScratchMemory_Alloc(
//       internal_sprites_raw_memory, sizeof(ScratchInternalSprite) *
//       num_sprites);

//   ScratchAllSprites all_sprites =
//       ScratchAllSprites_Init(&all_sprites_memory_buffer, num_sprites);

//   ScratchAllSprites_InitSingleSprite(&all_sprites, 0, &sprite0);
//   ScratchAllSprites_InitSingleSprite(&all_sprites, 1, &sprite1);

//   size_t memory_used = ScratchMemory_UsedManagedMemory();

//   ScratchSprite* s0 = ScratchAllSprites_GetFirst(&all_sprites);
//   for (int i = 0; i < SCRATCH_VM_NUM_PREALLOCATED_CLONES; ++i) {
//     ScratchSprite* s = ScratchSprite_Clone(s0);
//     s->costume_number = i + 1;
//   }

//   ScratchSprite* cur_sprite = ScratchAllSprites_GetFirst(&all_sprites);
//   for (int i = 0; i < SCRATCH_VM_NUM_PREALLOCATED_CLONES + 1; ++i) {
//     ASSERT_EQ(cur_sprite->position.x, 0);
//     ASSERT_EQ(cur_sprite->position.y, 0);
//     ASSERT_EQ(cur_sprite->direction, 0);
//     ASSERT_EQ(cur_sprite->size, 0);
//     ASSERT_EQ(cur_sprite->costume_number, i);
//     cur_sprite = ScratchAllSprites_GetNext(&all_sprites, cur_sprite);
//   }

//   ASSERT_EQ(cur_sprite->position.x, 1);
//   ASSERT_EQ(cur_sprite->position.y, 1);
//   ASSERT_EQ(cur_sprite->direction, 1);
//   ASSERT_EQ(cur_sprite->size, 1);
//   ASSERT_EQ(cur_sprite->costume_number,
//             SCRATCH_VM_NUM_PREALLOCATED_CLONES + 10);

//   ASSERT_GT(ScratchMemory_UsedManagedMemory() - memory_used, 0);
// }
