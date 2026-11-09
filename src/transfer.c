#include "../include/tree_arena.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

Arena_String string_store_testing(Arena *arena, char *string_to_copy) {
    // Initiate an Arena_String struct
    Arena_String arena_string = {0};

    // Determine the string's size
    size_t string_size = strlen(string_to_copy)+1;

    size_t string_offset = arena->current_offset;

    // Determine where in the arena the string will start
    void *string_ptr = arena_alloc_align(arena, string_size, DEFAULT_ALIGNMENT);
    if (string_ptr == NULL) {
        return arena_string;
    }

    // Copy the string into the arena's byte buffer
    memcpy(string_ptr, string_to_copy, string_size);

    // Assign the string's metadata to the `arena_string` struct
    arena_string.string_ptr = string_ptr;
    arena_string.string_offset = string_offset;
    arena_string.string_size = string_size;

    return arena_string;
}

char *string_retrieve_testing(Arena *arena, Arena_String arena_string) {
    char *string_result = (char *)arena_string.string_ptr;

    return string_result;
}

int main(void) {
    // Initiate an empty arena
    Arena arena = {0};
    // We're just gonna start with an excessive amount of memory allocation
    // We'll worry about narrowing down the memory usage after a basic
    // implementation has been achieved
    // INITIALIZE A WAY-TOO-BIG ARENA
    byte arena_buffer[1024];
    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    arena_init(&arena, arena_buffer, arena_buffer_length);

    // Define the testing directory
    char *small_ex_dir = "SmallDir";

    // Initialize the root for the tree structure to be stored in the arena
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(root, small_ex_dir);
    tree_print(root, 0);

    void *string_ptr = arena_string_store(&arena, "Hello there, Mr.Meow");
    char *string_result = arena_string_retrieve(&arena, string_ptr);

    printf("%s\n", string_result);

    return 0;
}
