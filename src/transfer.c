#include "../include/arena_tree.h"

int main(void) {
    Arena arena = {0};
    Arena arena_temp = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[1024]; // Max of 1024 chars for a file path (plenty of space)

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
    // This should be unsigned int, not size_t
    size_t count = sizeof(strings) / sizeof(strings[0]);
    size_t string_size = arena_string_dynamic_array_size(strings, strings_len);
    void *result = arena_string_dynamic_array(&arena_temp, strings, string_size, strings_len);
    printf("%s\n", (char *)result);

    return 0;
}
