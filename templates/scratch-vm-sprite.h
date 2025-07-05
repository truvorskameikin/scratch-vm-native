#ifndef __SCRATCH_VM_INCLUDE_SPRITE_H__
#define __SCRATCH_VM_INCLUDE_SPRITE_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-linked-list.h"
#include "scratch-vm-sprite-types.h"
#include "scratch-vm-variable.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScratchInternalSprite {
  ScratchSprite _;
  ScratchListNode all_sprites_entry;
  ScratchListNode my_clones_entry;
  size_t all_sprites_parent_index;
  ScratchMemoryBuffer owning_buffer;
  size_t num_variables;
  ScratchMemoryBuffer variables_buffer;  // Buffer of ScratchVariable.
} ScratchInternalSprite;

typedef struct ScratchAllSprites {
  ScratchList all_sprites_list;
  size_t num_parent_sprites;
  ScratchMemoryBuffer
      parent_pointers_sprites_buffer;       // Buffer of ScratchInternalSprite
                                            // pointers.
  ScratchMemoryBuffer clones_lists_buffer;  // Buffer of ScratchList.
} ScratchAllSprites;

size_t ScratchInternalSprite_GetMinBufferSize(size_t num_variables);
ScratchInternalSprite* ScratchInternalSprite_InitInplace(
    ScratchMemoryBuffer owning_buffer, size_t num_variables);
ScratchInternalSprite* ScratchInternalSprite_CloneInplace(
    ScratchMemoryBuffer owning_buffer, ScratchInternalSprite* to_clone);
void ScratchInternalSprite_InitVariable(ScratchInternalSprite* internal_sprite,
                                        size_t variable_index,
                                        ScratchVariable* rhv);

size_t ScratchAllSprites_GetMinParentPointersBufferSize(
    size_t num_parent_sprites);
size_t ScratchAllSprites_GetMinClonesListsBufferSize(size_t num_parent_sprites);
ScratchAllSprites ScratchAllSprites_Init(
    size_t num_parent_sprites,
    ScratchMemoryBuffer parent_pointers_sprites_buffer,
    ScratchMemoryBuffer clones_lists_buffer);
void ScratchAllSprites_AddParentSprite(ScratchAllSprites* all_sprites,
                                       size_t parent_sprite_index,
                                       ScratchInternalSprite* internal_sprite);
ScratchInternalSprite* ScratchAllSprites_Clone(ScratchAllSprites* all_sprites,
                                               ScratchInternalSprite* to_clone);
void ScratchAllSprites_DeleteClone(ScratchAllSprites* all_sprites,
                                   ScratchInternalSprite* clone);
// These functions will be used to provide sprites to VM clients. That is why
// they provide pointers to ScratchSprite.
ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites);
ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
                                         ScratchSprite* cur_sprite);
// These functions will be used to send broadcasts inside of VM. That why they
// return pointers to ScratchInternalSprite.
ScratchInternalSprite* ScratchAllSprites_GetFirstClone(
    ScratchAllSprites* all_sprites, size_t parent_sprite_index);
ScratchInternalSprite* ScratchAllSprites_GetNextClone(
    ScratchAllSprites* all_sprites, size_t parent_sprite_index,
    ScratchInternalSprite* cur_sprite);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_SPRITE_H__
