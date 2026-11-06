#include "../include/tree_arena.h"

int main(void) {
    // Define an array of 3 source directories
    // const char *source_dirs[] = {"TestDirs/Code", "TestDirs/Documents"};
    // size_t source_dirs_len = sizeof(source_dirs) / sizeof(source_dirs[0]);

    // // Define a target directory (leaving as an array for later implementations)
    // const char *target_dirs[] = {"TestDirs/TargetDir"};
    // size_t target_dir_len = sizeof(target_dirs) / sizeof(target_dirs[0]);

    // for (size_t i = 0; i < source_dirs_len; i++) {
    //     dirs_list_all(source_dirs[i]);
    // }

    // Let's begin with a small example directory

    // Now let's create an arena to store this directory.
    // We'll give it a bunch of space for preliminary testing
    Arena ex_arena;
    byte ex_arena_buffer[1024];
    size_t ex_arena_buffer_length = sizeof(ex_arena_buffer) / sizeof(ex_arena_buffer[0]);

    arena_init(&ex_arena, ex_arena_buffer, ex_arena_buffer_length);
    arena_alloc_align(&ex_arena, sizeof(ex_arena_buffer), DEFAULT_ALIGNMENT);

    // Now we need to rewrite the `directory_traverse` function to actually return something
    // Probably gonna want to do this from the ground up
    // First thing we need to focus on for this new function, is that we want to somehow store
    // the directory structure into a tree

    // We'll begin by making a tree by hand
    char *small_ex_dir = "TestDirs/SmallDir";
    Node *root = tree_create(small_ex_dir);

    return 0;
}
