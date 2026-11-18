#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_ALIGNMENT sizeof(void *)

typedef struct {
    unsigned char *buffer;
    size_t current_offset;
    size_t buffer_length;
} Arena;

// Initialization and allocation
void arena_init(Arena *arena, void *buffer, size_t buffer_length);
bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_allocate(Arena *arena, size_t size, size_t align);
// Storage and retrieval
void *arena_string_duplicate(Arena *arena, char *string, size_t string_size);
char *arena_string_concat(Arena *arena, char **string_array, size_t element_count);
// Checkpoint handling
size_t arena_checkpoint(Arena *arena);
void arena_rewind(Arena *arena, size_t checkpoint);

#endif
