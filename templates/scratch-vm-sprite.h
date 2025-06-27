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
    ScratchMemoryBuffer internal_sprite_buffer, size_t num_variables);
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
ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites);
ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
                                         ScratchSprite* cur_sprite);

// #if !defined(SCRATCH_VM_NUM_PREALLOCATED_CLONES)
// #define SCRATCH_VM_NUM_PREALLOCATED_CLONES 8
// #endif

// #if !defined(SCRATCH_VM_NUM_NEXT_CLONES)
// #define SCRATCH_VM_NUM_NEXT_CLONES 16
// #endif

// #define __CLONES_RAW_MEMORY_SIZE__ \
//   (SCRATCH_VM_NUM_PREALLOCATED_CLONES * sizeof(ScratchSpriteListNode))

// typedef struct ScratchSpriteListNode {
//   ScratchBufferedListNode _;
//   ScratchSprite sprite;
// } ScratchSpriteListNode;

// typedef struct ScratchInternalSprite {
//   size_t index;
//   ScratchBufferedList clones;
//   char clones_raw_memory[__CLONES_RAW_MEMORY_SIZE__];
// } ScratchInternalSprite;

// typedef struct ScratchAllSprites {
//   size_t num_sprites;
//   ScratchMemoryBuffer internal_sprites_array;
// } ScratchAllSprites;

// ScratchSprite* ScratchSprite_Clone(ScratchSprite* cur_sprite);
// void ScratchSprite_Delete(ScratchSprite* cur_sprite);

// ScratchAllSprites ScratchAllSprites_Init(ScratchMemoryBuffer* sprites_array,
//                                          size_t num_sprites);
// void ScratchAllSprites_InitSingleSprite(ScratchAllSprites* all_sprites,
//                                         size_t internal_sprite_index,
//                                         ScratchSprite* sprite_ref);
// ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites);
// ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
//                                          ScratchSprite* cur_sprite);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_SPRITE_H__
