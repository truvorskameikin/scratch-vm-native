#pragma once

{% include 'scratch-vm-types.h' with context %}

{% include 'scratch-vm-variables-public.h' with context %}

#ifdef __cplusplus
extern "C" {
#endif

void Scratch_Init(void);
void Scratch_Advance(ScratchNumber dt);

#ifdef __cplusplus
}
#endif

// Need two new lines in the end.

