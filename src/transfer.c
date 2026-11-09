#include "../include/arena_tree.h"

int main(void) {
    // Initiate an empty arena
    Arena arena = {0};
    // We're just gonna start with an excessive amount of memory allocation
    // We'll worry about narrowing down the memory usage after a basic
    // implementation has been achieved
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[1024]; // Ensure buffer is aligned
    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    arena_init(&arena, arena_buffer, arena_buffer_length);

    // Define a testing directory
    char *small_ex_dir = "SmallDir";

    // Initialize the root for the tree structure to be stored in the arena
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, root, small_ex_dir);
    tree_print(root, 0);

    return 0;
}
