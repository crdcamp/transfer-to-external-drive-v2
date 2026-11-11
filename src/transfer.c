#include <limits.h>

#include "../include/arena_tree.h"

int main(void) {
    Arena arena = {0};
    Arena arena_temp = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[131072];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[PATH_MAX * 64];

    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    size_t arena_temp_buffer_length = sizeof(arena_temp_buffer) / sizeof(arena_temp_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&arena_temp, arena_temp_buffer, arena_temp_buffer_length);

    // We're just testing a smaller file structure for now
    // TODO: Add a check for if the directory exists
    char *source_dir = "TestDirs/Code";

    // Initialize the root_source_dir for the tree structure to be stored in the arena
    Node *root_source_dir = new_node(&arena, source_dir);
    if (root_source_dir == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, &arena_temp, root_source_dir, source_dir);
    tree_print(root_source_dir, 0);
    arena_reset(&arena_temp);

    return 0;
}
