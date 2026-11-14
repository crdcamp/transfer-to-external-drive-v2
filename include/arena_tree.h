#ifndef ARENA_TREE_H
#define ARENA_TREE_H

#include <sys/mman.h>

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <dirent.h>
#include <copyfile.h>

#define DEFAULT_ALIGNMENT sizeof(void *)

///// ARENA /////
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
// Resetting offsets to 0

///// TREE /////
typedef struct Node {
    char *name; // The name of the directory
    struct Node *parent; // The current directory's parent
    struct Node *first_child; // The current directory's subdirectory
    struct Node *next_sibling; // The current directory's next subdirectory
} Node;

Node *new_node(Arena *arena, char *name);
void tree_fill(Arena *arena, Arena *arena_scratch, Node *node, char *file_path);
bool tree_is_identical(Node *root_source, Node *root_target);
void tree_list_node_directories(char *file_path);

void tree_print(Node *root, int depth); // Remove this when project is complete

void list_directories(char *file_path);
void print_arena_info(Arena *arena);
void tree_get_filepaths(Node *node, Arena *arena_scratch, char *base_path);

void tree_copy_identical(Arena *arena_scratch, Node *source_node, char *source_base_path, Node *target_node, char *target_base_path);
//void tree_copy_different();

#endif
