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
void *arena_string_duplicate(Arena *arena, char *string, size_t string_size);
char *arena_string_concat(Arena *arena, char **string_array, size_t element_count);
// Resetting offsets to 0
void arena_reset(Arena *arena);


///// TREE /////
typedef struct Node {
    char *name; // The name of the directory
    char **files; // An array of the file names in the directory
    struct Node *parent; // The current directory's parent
    struct Node *first_child; // The current directory's subdirectory
    struct Node *next_sibling; // The current directory's next subdirectory
} Node;

char **list_files(Arena *arena, Arena arena_temp, char *file_path);
Node *new_node(Arena *arena, char *name);
void tree_fill(Arena *arena, Arena *arena_temp, Node *node, char *file_path);
void tree_print(Node *root, int depth);

#endif
