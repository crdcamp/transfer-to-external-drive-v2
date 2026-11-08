#ifndef TREE_ARENA_H
#define TREE_ARENA_H

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
void tree_print(Node *root, int depth);

// ARENA FUNCTIONALITY
#define DEFAULT_ALIGNMENT (2*sizeof(void *))

typedef unsigned char byte;

typedef struct {
    byte *buffer;
    size_t previous_offset;
    size_t current_offset;
    size_t buffer_length;
} Arena;

// From Tsoding's example: https://github.com/tsoding/arena/blob/master/examples/001_basic_usage/main.c
// StringBuilder is any structure that has at least three fields: items, count, and capacity
// - `items` MUST be a pointer to char and it points to the beginning of the buffer that stores the string
// - `count` MUST be size_t and it indicates the size of the string
// - `capacity` MUST be size_t and it indicates how much memory was already allocated for the StringBuilder
// StringBuilder in a sense is just a dynamic array of characters
typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

void arena_init(Arena *arena, void *buffer, size_t buffer_length);
bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_alloc_align(Arena *arena, size_t size, size_t align);
void arena_free(Arena *arena);

#endif
