#include "../include/tree_arena.h"

int main(void) {
    // For MUCH LATER use
    Arena ex_arena;
    // We're just gonna start with an excessive amount of memory allocation
    // However, I have a feeling this should be done within the `tree_fill` function
    byte ex_arena_buffer[1024];
    size_t ex_arena_buffer_length = sizeof(ex_arena_buffer) / sizeof(ex_arena_buffer[0]);

    arena_init(&ex_arena, ex_arena_buffer, ex_arena_buffer_length);
    arena_alloc_align(&ex_arena, sizeof(ex_arena_buffer), DEFAULT_ALIGNMENT);


    char *small_ex_dir = "SmallDir";
    Node *root = new_node(small_ex_dir);
    if (root == NULL) return 1;

    tree_fill(root, small_ex_dir);
    tree_print(root, 0);

    return 0;
}
