#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include "scratch-vm-sprite.h"

#include <assert.h>
#include <stddef.h>
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

static ScratchInternalSprite* ScratchInternalSprite_GetPointerFromMyClonesNode(
    ScratchListNode* list_node) {
  if (list_node == 0) {
    return 0;
  }

  size_t my_clones_entry_offset =
      offsetof(ScratchInternalSprite, my_clones_entry);
  return (ScratchInternalSprite*)(((char*)list_node) - my_clones_entry_offset);
}

ScratchInternalSprite* ScratchInternalSprite_InitInplace(
    ScratchMemoryBuffer owning_buffer, size_t num_variables) {
  assert(ScratchInternalSprite_GetMinBufferSize(num_variables) <=
         owning_buffer.size);

  ScratchInternalSprite* internal_sprite =
      (ScratchInternalSprite*)owning_buffer.buffer;
  internal_sprite->owning_buffer = owning_buffer;
  internal_sprite->num_variables = num_variables;
  internal_sprite->variables_buffer = ScratchMemory_AllocInplace(
      (char*)owning_buffer.buffer + sizeof(ScratchInternalSprite),
      sizeof(ScratchVariable) * num_variables);

  return internal_sprite;
}

ScratchInternalSprite* ScratchInternalSprite_CloneInplace(
    ScratchMemoryBuffer owning_buffer, ScratchInternalSprite* to_clone) {
  assert(ScratchInternalSprite_GetMinBufferSize(to_clone->num_variables) <=
         owning_buffer.size);

  ScratchInternalSprite* internal_sprite =
      (ScratchInternalSprite*)owning_buffer.buffer;
  internal_sprite->all_sprites_parent_index =
      to_clone->all_sprites_parent_index;
  internal_sprite->owning_buffer = owning_buffer;
  internal_sprite->num_variables = to_clone->num_variables;
  internal_sprite->variables_buffer = ScratchMemory_AllocInplace(
      (char*)owning_buffer.buffer + sizeof(ScratchInternalSprite),
      sizeof(ScratchVariable) * to_clone->num_variables);

  ScratchVariable* variables = internal_sprite->variables_buffer.buffer;
  ScratchVariable* to_clone_variables = to_clone->variables_buffer.buffer;

  for (size_t i = 0; i < to_clone->num_variables; ++i) {
    Scratch_AssignVariable(&variables[i], &to_clone_variables[i]);
  }

  ScratchSprite* dest_sprite = (ScratchSprite*)internal_sprite;
  ScratchSprite* src_sprite = (ScratchSprite*)to_clone;
  *dest_sprite = *src_sprite;

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
  internal_sprite->all_sprites_parent_index = parent_sprite_index;

  ScratchInternalSprite** parent_sprites =
      (ScratchInternalSprite**)
          all_sprites->parent_pointers_sprites_buffer.buffer;
  parent_sprites[parent_sprite_index] = internal_sprite;

  ScratchList_InsertBack(&all_sprites->all_sprites_list,
                         &internal_sprite->all_sprites_entry);

  ScratchList* clones_lists = all_sprites->clones_lists_buffer.buffer;
  ScratchList_InsertBack(&clones_lists[parent_sprite_index],
                         &internal_sprite->my_clones_entry);
  ScratchSprite* sprite = (ScratchSprite*)internal_sprite;
  sprite->is_clone = 0;
}

ScratchInternalSprite* ScratchAllSprites_Clone(
    ScratchAllSprites* all_sprites, ScratchInternalSprite* to_clone) {
  ScratchMemoryBuffer owning_buffer = ScratchMemory_Alloc(
      /*unmanaged_buffer=*/0,
      ScratchInternalSprite_GetMinBufferSize(to_clone->num_variables));
  ScratchInternalSprite* clone =
      ScratchInternalSprite_CloneInplace(owning_buffer, to_clone);

  ScratchList_InsertBack(&all_sprites->all_sprites_list,
                         &clone->all_sprites_entry);

  ScratchList* clones_lists = all_sprites->clones_lists_buffer.buffer;
  ScratchList_InsertFront(&clones_lists[clone->all_sprites_parent_index],
                          &clone->my_clones_entry);

  ScratchSprite* sprite = (ScratchSprite*)clone;
  sprite->is_clone = 1;

  return clone;
}

void ScratchAllSprites_DeleteClone(ScratchAllSprites* all_sprites,
                                   ScratchInternalSprite* clone) {
  ScratchSprite* sprite = (ScratchSprite*)clone;
  if (!sprite->is_clone) {
    return;
  }

  ScratchList_Remove(&all_sprites->all_sprites_list, &clone->all_sprites_entry);
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

ScratchInternalSprite* ScratchAllSprites_GetFirstClone(
    ScratchAllSprites* all_sprites, size_t parent_sprite_index) {
  ScratchList* clones_lists = all_sprites->clones_lists_buffer.buffer;
  ScratchListNode* list_node =
      ScratchList_GetFirst(&clones_lists[parent_sprite_index]);
  return ScratchInternalSprite_GetPointerFromMyClonesNode(list_node);
}

ScratchInternalSprite* ScratchAllSprites_GetNextClone(
    ScratchAllSprites* all_sprites, size_t parent_sprite_index,
    ScratchInternalSprite* cur_sprite) {
  ScratchList* clones_lists = all_sprites->clones_lists_buffer.buffer;
  ScratchListNode* next_list_node = ScratchList_GetNext(
      &clones_lists[parent_sprite_index], &cur_sprite->my_clones_entry);
  return ScratchInternalSprite_GetPointerFromMyClonesNode(next_list_node);
}
