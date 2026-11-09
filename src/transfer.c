#include "../include/tree_arena.h"
#include <cstring>
#include <stdint.h>
#include <string.h>

// Should we make a function that finds the longest file path
// amongst the input and output directories?

// That way, we can have the small arena be the exact length we
// need it to be and avoid wasting additional memory (albeit
// a very small amount, but the whole point of this project is
// a strong focus on performance)
// ^A BETTER IDEA FOR THIS IS ADDRESSED IN `tree_fill` IN `tree.c`

// How would this function work?
// Well, we're passing arrays of file paths as sources, but that
// doesn't mean the function should be accepting an array of strings
// Instead, it should be called individually on each file path

// However, this adds some more complexity when creating the function,
// and also brings in the need for iterating through the entire tree
// twice

// We could find the longest assembly of nodes in the tree, but this doesn't
// guarantee the longest file path, as this wouldn't account for string characters

// A lazy approach would be to call `realpath` all over the place, but that's
// a pretty unacceptable approach....

// Welp... I think the best option is to just start by allocating plenty of
// space upfront. Determining how to narrow down that space should be a
// problem for later anyway. Let's do that for now then

// Probably the best approach would be to just keep everything in the same
// arena and free these string function replacements right after they're
// done being used

// Bill also gives an example of TEMPORARY ARENA MEMORY FUNCTIONS
// that are definitely perfect for this use case. We'll work on that
// after messing around with some more basic implementations first
// Or, we might not need a temporary arena if we can resize the arena
// later on

// USING TSODING'S EXAMPLE TO FIGURE THIS OUT:
// https://github.com/tsoding/arena/blob/master/examples/001_basic_usage/main.c
ArenaString strdup_arena_testing(Arena *arena, char *string_to_copy) {
    // Initiate an ArenaString struct
    ArenaString arena_string = {0};

    // Determine the size in bytes of the string
    size_t string_size = strlen(string_to_copy)+1;

    // Assign the string size to the struct
    arena_string.size = string_size;

    // Allocate memory to the arena and return a pointer to where the memory begins
    arena_string.string_offset = (uintptr_t) arena_alloc_align(arena, string_size, DEFAULT_ALIGNMENT);

    return arena_string;
}

void asprintf_arena_testing(Arena *arena, char *strings_to_concatenate) {
    return;
}

int main(void) {
    // For MUCH LATER use
    Arena ex_arena = {0};
    // We're just gonna start with an excessive amount of memory allocation
    // We'll worry about narrowing down the memory usage after a basic
    // implementation has been achieved
    // INITIALIZE A WAY-TOO-BIG ARENA
    byte ex_arena_buffer[1024];
    size_t ex_arena_buffer_length = sizeof(ex_arena_buffer) / sizeof(ex_arena_buffer[0]);
    arena_init(&ex_arena, ex_arena_buffer, ex_arena_buffer_length);

    // DEFINE A TESTING DIRECTORY
    char *small_ex_dir = "SmallDir";

    // INITIALIZE TREE STRUCTURE TO BE STORED IN ARENA
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // CREATE A TREE THAT ACTS AS METADATA FOR THE DIRECTORY STRUCTURE
    tree_fill(root, small_ex_dir);
    tree_print(root, 0);

    // Now we're just messing around and figuring out next steps
    strdup_arena_testing(&ex_arena, "Meow hello hi how are you?");

    return 0;
}
