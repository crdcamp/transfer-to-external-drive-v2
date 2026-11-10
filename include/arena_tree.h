#ifndef ARENA_TREE_H
#define ARENA_TREE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>

///// ARENA /////
#define DEFAULT_ALIGNMENT (2*sizeof(void *))

typedef unsigned char byte;

typedef struct {
    byte *buffer;
    size_t previous_offset;
    size_t current_offset;
    size_t buffer_length;
} Arena;

// Initialization and allocation
void arena_init(Arena *arena, void *buffer, size_t buffer_length);
bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_allocate(Arena *arena, size_t size, size_t align);
// Storage and retrieval
void *arena_string_store(Arena *arena, char *string, size_t string_size);
void *arena_string_duplicate(Arena *arena, char *string, size_t string_size);
char *arena_concat_file_paths(Arena *arena, char *string_1, char *string_2, char delim, size_t total_size);
void arena_string_reset(Arena *arena, void *string_ptr, size_t string_size);
// Reset the arena's offsets to 0
void arena_reset(Arena *arena);


///// TREE /////
typedef struct Node {
    char *name;
    struct Node *parent;
    struct Node *first_child;
    struct Node *next_sibling;
} Node;

Node *new_node(char *name);
void tree_fill(Arena *arena, Node *node, char *file_path);
void tree_print(Node *root, int depth); // Needs some fixing

#endif
