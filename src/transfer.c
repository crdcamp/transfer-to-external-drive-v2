#include "../include/tree_arena.h"

// Should we make a function that finds the longest file path
// amongst the input and output directories?

// That way, we can have the small arena be the exact length we
// need it to be and avoid wasting additional memory (albeit
// a very small amount, but the whole point of this project is
// a focus on performance)

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
// after messing around here for a bit

void strdup_arena_testing(Arena *arena, char *string_to_copy) {
    size_t string_len = strlen(string_to_copy);
    printf("Size of string: %zu byte(s)\n", string_len);
}

void asprintf_arena_testing(Arena *arena, char *strings_to_concatenate) {
    return;
}

int main(void) {
    // For MUCH LATER use
    Arena ex_arena;
    // We're just gonna start with an excessive amount of memory allocation
    // However, I have a feeling this should be done within the `tree_fill` function
    byte ex_arena_buffer[1024];
    size_t ex_arena_buffer_length = sizeof(ex_arena_buffer) / sizeof(ex_arena_buffer[0]);

    arena_init(&ex_arena, ex_arena_buffer, ex_arena_buffer_length);
    arena_alloc_align(&ex_arena, sizeof(ex_arena_buffer), DEFAULT_ALIGNMENT);


    char *small_ex_dir = "SmallDir";
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    tree_fill(root, small_ex_dir);
    tree_print(root, 0);

    strdup_arena_testing(&ex_arena, "Meow hello hi how are you?");

    return 0;
}
