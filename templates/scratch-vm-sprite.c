#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-sprite.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#endif

size_t ScratchInternalSprite_GetMinBufferSize(size_t num_variables) {
  return sizeof(ScratchInternalSprite) +
         sizeof(ScratchVariable) * num_variables;
}

size_t ScratchAllSprites_GetMinParentPointersBufferSize(
    size_t num_parent_sprites) {
  return sizeof(ScratchInternalSprite*) * num_parent_sprites;
}

static ScratchInternalSprite*
ScratchInternalSprite_GetPointerFromAllSpritesNode(ScratchListNode* list_node) {
  if (list_node == 0) {
    return 0;
  }

  size_t all_sprites_entry_offset =
      offsetof(ScratchInternalSprite, all_sprites_entry);
  return (ScratchInternalSprite*)(((char*)list_node) -
                                  all_sprites_entry_offset);
}

ScratchInternalSprite* ScratchInternalSprite_InitInplace(
    ScratchMemoryBuffer internal_sprite_buffer, size_t num_variables) {
  assert(ScratchInternalSprite_GetMinBufferSize(num_variables) <=
         internal_sprite_buffer.size);

  ScratchInternalSprite* internal_sprite =
      (ScratchInternalSprite*)internal_sprite_buffer.buffer;
  internal_sprite->num_variables = num_variables;
  internal_sprite->variables_buffer = ScratchMemory_AllocInplace(
      (char*)internal_sprite_buffer.buffer + sizeof(ScratchInternalSprite),
      sizeof(ScratchVariable) * num_variables);

  return internal_sprite;
}

void ScratchInternalSprite_InitVariable(ScratchInternalSprite* internal_sprite,
                                        size_t variable_index,
                                        ScratchVariable* rhv) {
  ScratchVariable* variables = internal_sprite->variables_buffer.buffer;
  Scratch_AssignVariable(&variables[variable_index], rhv);
}

size_t ScratchAllSprites_GetMinClonesListsBufferSize(
    size_t num_parent_sprites) {
  return sizeof(ScratchList) * num_parent_sprites;
}

ScratchAllSprites ScratchAllSprites_Init(
    size_t num_parent_sprites,
    ScratchMemoryBuffer parent_pointers_sprites_buffer,
    ScratchMemoryBuffer clones_lists_buffer) {
  assert(ScratchAllSprites_GetMinParentPointersBufferSize(num_parent_sprites) <=
         parent_pointers_sprites_buffer.size);
  assert(ScratchAllSprites_GetMinClonesListsBufferSize(num_parent_sprites) <=
         clones_lists_buffer.size);

  ScratchAllSprites result;
  result.all_sprites_list = ScratchList_Init();

  result.num_parent_sprites = num_parent_sprites;
  result.parent_pointers_sprites_buffer = parent_pointers_sprites_buffer;
  result.clones_lists_buffer = clones_lists_buffer;

  ScratchList* clones_lists = (ScratchList*)clones_lists_buffer.buffer;
  for (size_t i = 0; i < num_parent_sprites; ++i) {
    clones_lists[i] = ScratchList_Init();
  }

  return result;
}

void ScratchAllSprites_AddParentSprite(ScratchAllSprites* all_sprites,
                                       size_t parent_sprite_index,
                                       ScratchInternalSprite* internal_sprite) {
  ScratchInternalSprite** parent_sprites =
      (ScratchInternalSprite**)
          all_sprites->parent_pointers_sprites_buffer.buffer;
  parent_sprites[parent_sprite_index] = internal_sprite;

  ScratchList_InsertBack(&all_sprites->all_sprites_list,
                         &internal_sprite->all_sprites_entry);
}

ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites) {
  ScratchListNode* list_node =
      ScratchList_GetFirst(&all_sprites->all_sprites_list);
  return (ScratchSprite*)ScratchInternalSprite_GetPointerFromAllSpritesNode(
      list_node);
}

ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
                                         ScratchSprite* cur_sprite) {
  ScratchInternalSprite* internal_sprite = (ScratchInternalSprite*)cur_sprite;
  ScratchListNode* next_list_node = ScratchList_GetNext(
      &all_sprites->all_sprites_list, &internal_sprite->all_sprites_entry);
  return (ScratchSprite*)ScratchInternalSprite_GetPointerFromAllSpritesNode(
      next_list_node);
}

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
