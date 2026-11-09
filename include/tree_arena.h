#ifndef TREE_ARENA_H
#define TREE_ARENA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>

// TREE FUNCTIONALITY
typedef struct Node {
    char *name;
    struct Node *parent;
    struct Node *first_child;
    struct Node *next_sibling;
} Node;

Node *new_node(char *name);
void tree_fill(Node *node, char *file_path);
void tree_print(Node *root, int depth); // Needs some fixing

// ARENA FUNCTIONALITY
#define DEFAULT_ALIGNMENT (2*sizeof(void *))

typedef unsigned char byte;

typedef struct {
    byte *buffer;
    size_t previous_offset;
    size_t current_offset;
    size_t buffer_length;
} Arena;

typedef struct {
    size_t string_size;
    void *string_ptr; // Where the string begins in the arena's memory
} Arena_String;

// Initialization and allocation
void arena_init(Arena *arena, void *buffer, size_t buffer_length);
bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_alloc_align(Arena *arena, size_t size, size_t align);
// Storage and retrieval
void *arena_string_store(Arena *arena, char *string);
char *arena_string_retrieve(Arena *arena, void *string_ptr);
Arena_String arena_string_duplicate(Arena *arena, char *string);
void arena_string_reset(Arena *arena, void *string_ptr);
// Reset the arena's offsets to 0
void arena_reset(Arena *arena);

#endif
