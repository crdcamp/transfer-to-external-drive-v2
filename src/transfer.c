#include "../include/arena_tree.h"

int main(void) {
    Arena arena = {0};
    Arena arena_temp = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[256]; // Max of 256 chars should be enough space for very long file path

    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    size_t arena_temp_buffer_length = sizeof(arena_temp_buffer) / sizeof(arena_temp_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&arena_temp, arena_temp_buffer, arena_temp_buffer_length);

    // We're just testing for now
    char *small_ex_dir = "TestDirs/SmallDir";

    // Initialize the root for the tree structure to be stored in the arena
    // (The tree arena storage has not been implemented yet)
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, root, small_ex_dir);
    tree_print(root, 0);

    char *strings[] = {"meow", "/", "beepboopbop", "/", "bingbangpow"};
    size_t element_count = sizeof(strings) / sizeof(strings[0]);
    char *result = arena_string_concat(&arena_temp, strings, element_count);
    printf("%s\n", result);

    return 0;
}
