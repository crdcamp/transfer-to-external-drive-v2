#include "../include/arena_tree.h"

int main(void) {
    Arena arena = {0};
    // Not sure if "scratch" is the right terminology here
    // regardless, this is meant for temporary allocations
    Arena arena_scratch = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_scratch_buffer[1024]; // Max of 1024 chars for a file path (plenty of space)

    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);

    // We're just testing for now
    char *small_ex_dir = "TestDirs/SmallDir";

    // Initialize the root for the tree structure to be stored in the arena
    // (The tree arena storage has not been implemented yet)
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, root, small_ex_dir);
    tree_print(root, 0);


    char *strings[] = {"ab", "bc", "cd"};
    //char *strings[] = {"meow", "/", "beepboop"};
    size_t strings_size = 1;
    size_t strings_len = sizeof(strings) / sizeof(strings[0]);
    for (size_t i = 0; i < strings_len; i++) {
        strings_size += strlen(strings[i]);
    }

    void *result = arena_dynamic_string_array(&arena_scratch, strings, strings_size);
    printf("%s\n", (char *)result);

    return 0;
}
