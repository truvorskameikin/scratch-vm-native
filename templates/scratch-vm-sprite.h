#ifndef __SCRATCH_VM_INCLUDE_SPRITE_H__
#define __SCRATCH_VM_INCLUDE_SPRITE_H__

#if !defined(__SCRATCH_VM_INSIDE_TEMPLATE__)
#include <stddef.h>

#include "scratch-vm-buffered-linked-list.h"
#include "scratch-vm-sprite-types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(SCRATCH_VM_NUM_PREALLOCATED_CLONES)
#define SCRATCH_VM_NUM_PREALLOCATED_CLONES 8
#endif

#if !defined(SCRATCH_VM_NUM_NEXT_CLONES)
#define SCRATCH_VM_NUM_NEXT_CLONES 16
#endif

#define __CLONES_RAW_MEMORY_SIZE__ \
  (SCRATCH_VM_NUM_PREALLOCATED_CLONES * sizeof(ScratchSpriteListNode))

typedef struct ScratchSpriteListNode {
  ScratchBufferedListNode _;
  ScratchSprite sprite;
} ScratchSpriteListNode;

typedef struct ScratchInternalSprite {
  size_t index;
  ScratchBufferedList clones;
  char clones_raw_memory[__CLONES_RAW_MEMORY_SIZE__];
} ScratchInternalSprite;

typedef struct ScratchAllSprites {
  size_t num_sprites;
  ScratchMemoryBuffer internal_sprites_array;
} ScratchAllSprites;

ScratchSprite* ScratchSprite_Clone(ScratchSprite* cur_sprite);
void ScratchSprite_Delete(ScratchSprite* cur_sprite);

ScratchAllSprites ScratchAllSprites_Init(ScratchMemoryBuffer* sprites_array,
                                         size_t num_sprites);
void ScratchAllSprites_InitSingleSprite(ScratchAllSprites* all_sprites,
                                        size_t internal_sprite_index,
                                        ScratchSprite* sprite_ref);
ScratchSprite* ScratchAllSprites_GetFirst(ScratchAllSprites* all_sprites);
ScratchSprite* ScratchAllSprites_GetNext(ScratchAllSprites* all_sprites,
                                         ScratchSprite* cur_sprite);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __SCRATCH_VM_INCLUDE_SPRITE_H__
