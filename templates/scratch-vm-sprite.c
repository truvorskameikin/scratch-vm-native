#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-sprite.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#endif

// static ScratchInternalSprite* ScratchSprite_GetInternalSprite(
//     ScratchSprite* sprite) {
//   return (ScratchInternalSprite*)sprite->internal_pointer_1;
// }

// static ScratchSpriteListNode* ScratchSprite_GetListNode(ScratchSprite*
// sprite) {
//   return (ScratchSpriteListNode*)sprite->internal_pointer_2;
// }

// // =====
// // ScratchSprite
// // =====
// ScratchSprite* ScratchSprite_Clone(ScratchSprite* cur_sprite) {
//   ScratchInternalSprite* internal_sprite =
//       ScratchSprite_GetInternalSprite(cur_sprite);

//   ScratchSpriteListNode* sprite_list_node =
//       (ScratchSpriteListNode*)ScratchBufferedList_InsertNew(
//           &internal_sprite->clones);

//   sprite_list_node->sprite = *cur_sprite;
//   sprite_list_node->sprite.internal_pointer_1 = internal_sprite;
//   sprite_list_node->sprite.internal_pointer_2 = sprite_list_node;

//   return &sprite_list_node->sprite;
// }

// void ScratchSprite_Delete(ScratchSprite* cur_sprite) {
//   ScratchInternalSprite* internal_sprite =
//       ScratchSprite_GetInternalSprite(cur_sprite);
//   ScratchSpriteListNode* list_node = ScratchSprite_GetListNode(cur_sprite);

//   ScratchBufferedList_Remove(&internal_sprite->clones,
//                              (ScratchBufferedListNode*)list_node);
// }

// // =====
// // ScratchAllSprites
// // =====
// ScratchAllSprites ScratchAllSprites_Init(ScratchMemoryBuffer* sprites_array,
//                                          size_t num_sprites) {
//   assert(sprites_array->size >= sizeof(ScratchInternalSprite) * num_sprites
//   &&
//          "Buffer should be big enough to contain all internal sprites.");

//   ScratchAllSprites result;
//   result.num_sprites = num_sprites;
//   result.internal_sprites_array = *sprites_array;

//   ScratchInternalSprite* internal_sprites =
//       (ScratchInternalSprite*)result.internal_sprites_array.buffer;

//   for (size_t i = 0; i < num_sprites; ++i) {
//     internal_sprites[i].index = i;

//     ScratchMemoryBuffer memory_buffer = ScratchMemory_AllocInplace(
//         internal_sprites[i].clones_raw_memory, __CLONES_RAW_MEMORY_SIZE__);
//     internal_sprites[i].clones = ScratchBufferedList_Init(
//         sizeof(ScratchSpriteListNode), SCRATCH_VM_NUM_PREALLOCATED_CLONES,
//         SCRATCH_VM_NUM_NEXT_CLONES, &memory_buffer);
//   }

//   return result;
// }

// void ScratchAllSprites_InitSingleSprite(ScratchAllSprites* all_sprites,
//                                         size_t internal_sprite_index,
//                                         ScratchSprite* sprite_ref) {
//   ScratchInternalSprite* internal_sprites =
//       (ScratchInternalSprite*)all_sprites->internal_sprites_array.buffer;

//   assert(ScratchBufferedList_Size(
//              &internal_sprites[internal_sprite_index].clones) == 0 &&
//          "InitSingleSprite also creates the first \"clone\".");

//   ScratchSpriteListNode* sprite_list_node =
//       (ScratchSpriteListNode*)ScratchBufferedList_InsertNew(
//           &internal_sprites[internal_sprite_index].clones);

//   sprite_list_node->sprite = *sprite_ref;
//   sprite_list_node->sprite.internal_pointer_1 =
//       &internal_sprites[internal_sprite_index];
//   sprite_list_node->sprite.internal_pointer_2 = sprite_list_node;
// }

// ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites) {
//   if (all_sprites->num_sprites == 0) {
//     return 0;
//   }

//   ScratchInternalSprite* internal_sprites =
//       (ScratchInternalSprite*)all_sprites->internal_sprites_array.buffer;
//   ScratchSpriteListNode* sprite_list_node =
//       (ScratchSpriteListNode*)ScratchBufferedList_GetFirst(
//           &internal_sprites[0].clones);

//   return &sprite_list_node->sprite;
// }

// ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
//                                          ScratchSprite* cur_sprite) {
//   ScratchInternalSprite* internal_sprite =
//       ScratchSprite_GetInternalSprite(cur_sprite);
//   ScratchSpriteListNode* list_node = ScratchSprite_GetListNode(cur_sprite);

//   ScratchSpriteListNode* next_list_node =
//       (ScratchSpriteListNode*)ScratchBufferedList_GetNext(
//           &internal_sprite->clones, (ScratchBufferedListNode*)list_node);
//   if (next_list_node) {
//     return &next_list_node->sprite;
//   }

//   size_t next_index = internal_sprite->index + 1;
//   if (next_index >= all_sprites->num_sprites) {
//     return 0;
//   }

//   ScratchInternalSprite* internal_sprites =
//       (ScratchInternalSprite*)all_sprites->internal_sprites_array.buffer;

//   ScratchSpriteListNode* first_list_node =
//       (ScratchSpriteListNode*)ScratchBufferedList_GetFirst(
//           &internal_sprites[next_index].clones);

//   return &first_list_node->sprite;
// }
