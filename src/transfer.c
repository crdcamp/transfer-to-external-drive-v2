#include "../include/arena_tree.h"

int main(void) {
    // Gonna give a large amount of memory for testing. Will dynamically adjust later
    Arena arena = {0};
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072]; // Ensure buffer is aligned on creation
    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    arena_init(&arena, arena_buffer, arena_buffer_length);

    // We're just testing for now
    char *small_ex_dir = "TestDirs/SmallDir";

    // Initialize the root for the tree structure to be stored in the arena
    // (The tree arena storage has not been implemented yet)
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, root, small_ex_dir);
    tree_print(root, 0);



    char *ex_string_1 = "meow";
    char *ex_string_2 = "beepboop";
    char delimiter = '/';

    size_t ex_str_1_size = strlen(ex_string_1);
    size_t ex_str_2_size = strlen(ex_string_2);
    size_t delim_size = sizeof(delimiter);

    size_t total_size = ex_str_1_size + ex_str_2_size + delim_size + 1;

    void *arena_str_ptr = arena_string_delimiter_concat(&arena, ex_string_1, ex_string_2, delimiter, total_size);

    return 0;
}
