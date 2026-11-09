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

    return 0;
}
