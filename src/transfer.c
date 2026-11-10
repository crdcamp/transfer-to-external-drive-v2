#include "../include/arena_tree.h"

int main(void) {
    Arena arena = {0};
    Arena scratch_arena = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072];
    _Alignas(DEFAULT_ALIGNMENT) byte scratch_arena_buffer[1024]; // Max of 1024 chars for a file path (plenty of space)

    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    size_t scratch_arena_buffer_length = sizeof(scratch_arena_buffer) / sizeof(scratch_arena_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&scratch_arena, scratch_arena_buffer, scratch_arena_buffer_length);

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
    char *delimiter = "/";
    char *ex_string_2 = "beepboop";

    void *result = arena_concat_file_paths(&arena, ex_string_1, ex_string_2, delimiter);
    printf("%s\n", (char *)result);

    return 0;
}
