#ifndef TREE_ARENA_H
#define TREE_ARENA_H

#include <stdio.h>
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
void tree_print(Node *root, int depth);

// ARENA FUNCTIONALITY
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
