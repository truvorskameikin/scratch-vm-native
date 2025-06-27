#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "templates/scratch-vm-sprite.h"

using ::testing::ElementsAre;

struct Sprite1 {
  ScratchInternalSprite internal_sprite;
  ScratchVariable variables[3];
} sprite1;

struct Sprite2 {
  ScratchInternalSprite internal_sprite;
  ScratchVariable variables[1];
} sprite2;

struct Sprite3 {
  ScratchInternalSprite internal_sprite;
  ScratchVariable variables[50];
} sprite3;

void InitSprites() {
  sprite1.internal_sprite._.position.x = 1;
  sprite1.internal_sprite._.position.y = 1;
  sprite1.internal_sprite._.direction = 1;
  sprite1.internal_sprite._.costume_number = 1;

  sprite2.internal_sprite._.position.x = 2;
  sprite2.internal_sprite._.position.y = 20;
  sprite2.internal_sprite._.direction = 2;
  sprite2.internal_sprite._.costume_number = 200;

  sprite3.internal_sprite._.position.x = 30;
  sprite3.internal_sprite._.position.y = 3;
  sprite3.internal_sprite._.direction = 30;
  sprite3.internal_sprite._.costume_number = 3;
}

std::vector<ScratchSprite*> toVector(ScratchAllSprites* all_sprites) {
  std::vector<ScratchSprite*> result;
  for (ScratchSprite* cur_sprite = ScratchAllSprites_GetFirst(all_sprites);
       cur_sprite != 0;
       cur_sprite = ScratchAllSprites_GetNext(all_sprites, cur_sprite)) {
    result.push_back(cur_sprite);
  }
  return result;
}

TEST(scratch_vm_sprite_gtest, all_sprites_init) {
  InitSprites();

  ScratchInternalSprite* internal_sprite_1 = ScratchInternalSprite_InitInplace(
      ScratchMemory_Alloc(&sprite1, sizeof(sprite1)),
      sizeof(sprite1.variables) / sizeof(ScratchVariable));

  ScratchInternalSprite* internal_sprite_2 = ScratchInternalSprite_InitInplace(
      ScratchMemory_Alloc(&sprite2, sizeof(sprite2)),
      sizeof(sprite2.variables) / sizeof(ScratchVariable));

  ScratchInternalSprite* internal_sprite_3 = ScratchInternalSprite_InitInplace(
      ScratchMemory_Alloc(&sprite3, sizeof(sprite3)),
      sizeof(sprite3.variables) / sizeof(ScratchVariable));

  ScratchInternalSprite* parent_pointers[3];
  ScratchList clones_of_parents[3];

  ScratchAllSprites all_sprites = ScratchAllSprites_Init(
      /*num_parent_sprites=*/3,
      ScratchMemory_Alloc(parent_pointers, sizeof(parent_pointers)),
      ScratchMemory_Alloc(clones_of_parents, sizeof(clones_of_parents)));

  ScratchAllSprites_AddParentSprite(&all_sprites, /*parent_sprite_index=*/0,
                                    internal_sprite_1);
  ScratchAllSprites_AddParentSprite(&all_sprites, /*parent_sprite_index=*/1,
                                    internal_sprite_2);
  ScratchAllSprites_AddParentSprite(&all_sprites, /*parent_sprite_index=*/2,
                                    internal_sprite_3);

  std::vector<ScratchSprite*> sprites = toVector(&all_sprites);
  
  ASSERT_EQ(sprites.size(), 3);
  std::vector<int> costumes(3);
  std::transform(sprites.begin(), sprites.end(), costumes.begin(),
                 [](auto sprite) { return sprite->costume_number; });
  ASSERT_THAT(costumes, ElementsAre(1, 200, 3));

  std::vector<double> positions_x(3);
  std::transform(sprites.begin(), sprites.end(), positions_x.begin(),
                 [](auto sprite) { return sprite->position.x; });
  ASSERT_THAT(positions_x, ElementsAre(1, 2, 30));
  
  std::vector<double> positions_y(3);
  std::transform(sprites.begin(), sprites.end(), positions_y.begin(),
                 [](auto sprite) { return sprite->position.y; });
  ASSERT_THAT(positions_y, ElementsAre(1, 20, 3));
}
