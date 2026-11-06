#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>

// Credit: https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir
#define DEFAULT_ALIGNMENT (2*sizeof(void *))

typedef unsigned char byte;

typedef struct Arena {
    byte *buffer;
    size_t previous_offset;
    size_t current_offset;
    size_t buffer_length;
} Arena;

void arena_init(Arena *arena, void *buffer, size_t buffer_length);
bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_alloc_align(Arena *arena, size_t size, size_t align);
void arena_free(Arena *arena);

#endif
