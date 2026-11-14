#include "../include/arena_tree.h"

int main(void) {
    // Create main arena using a virtual memory space
    Arena arena = {0};
    size_t size = 524288;
    void *buf = mmap(NULL, size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON,
                        -1, 0);
    if (buf == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    // Define buffer size for the source and target arenas
    size_t arena_scratch_buffer_size = PATH_MAX*64;

    // Create scratch arena using a normal buffer
    Arena arena_scratch = {0};
    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);

    // Initialize all main and scratch arenas
    arena_init(&arena, buf, size); // Main arena
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);

    // Define source and target directories
    char *source_dir = "TestDirs/TestSource";
    char *target_dir = "TestDirs/TestTarget"; // Testing with identical directories as a start

    // Initialize the source and target directory trees
    Node *source_root = new_node(&arena, source_dir);
    if (source_root == NULL) {
        printf("Error: `source_root` returned NULL on creation\n");
        return 1;
    }
    Node *target_root = new_node(&arena, target_dir);
    if (target_root == NULL) {
        printf("Error: `target_root` returned NULL on creation\n");
        return 1;
    }

    // Fill trees for the source and target directories
    tree_fill(&arena, &arena_scratch, source_root, source_dir);
    tree_fill(&arena, &arena_scratch, target_root, target_dir);

    bool is_identical = tree_is_identical(source_root->first_child, target_root->first_child);
    if (is_identical) printf("Trees are identical\n\n");
    else printf("Trees are not identical\n\n");

    tree_copy_identical(&arena, &arena_scratch, source_root, source_dir, target_root, target_dir);

    return 0;
}

void copyfile_testing(char *source, char *target) {
    copyfile(source, target, NULL, COPYFILE_DATA);
}
