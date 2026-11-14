#include "../include/arena_tree.h"

int main(void) {
    // Allocate about a half a megabyte of virtual memory
    Arena arena_main = {0};
    size_t size = 524288;
    void *buf = mmap(NULL, size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON,
                        -1, 0);
    if (buf == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    // Define buffer size for the source and target arenas
    size_t arena_scratch_buffer_size = PATH_MAX*64;
    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];

    // Define buffer lengths for all arenas
    Arena arena_scratch = {0};
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);

    // Initialize all arenas
    arena_init(&arena_main, buf, size); // Main arena
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length); // Scratch arena

    // TODO: Convert source directory to an array of directories (for much later)
    char *source_dir = "TestDirs/TestSource";
    char *target_dir = "TestDirs/TestTarget"; // Testing with identical directories as a start

    // Initialize the source and target directory trees
    Node *source_root = new_node(&arena_source, source_dir);
    if (source_root == NULL) {
        printf("Error: `source_root` returned NULL on creation\n");
        return 1;
    }
    Node *target_root = new_node(&arena_target, target_dir);
    if (target_root == NULL) {
        printf("Error: `target_root` returned NULL on creation\n");
        return 1;
    }

    // Fill trees for the source and target directories
    tree_fill(&arena_source, &arena_scratch, source_root, source_dir);
    tree_fill(&arena_target, &arena_scratch, target_root, target_dir);

    bool is_identical = tree_is_identical(source_root->first_child, target_root->first_child);
    if (is_identical) printf("Trees are identical\n\n");
    else printf("Trees are not identical\n\n");

    // If the trees are identical, overwrite ONLY the files within each directory
    // If the trees are not identical, overwrite the ENTIRE tree
    // We can think about more elegant solutions when these two requirements are met

    // I THINK THE PATH FORWARD SHOULD INVOLVE ALLOCATING VIRTUAL MEMORY SPACE
    // INSTEAD OF MAKING AN ARENA FOR EACH DIRECTORY TREE, JUST PUT THEM ALL IN
    // A BIG ARENA ASSIGNED AS VIRTUAL MEMORY SPACE (about 0.5mb) USE `mmap`

    // OR: ONE ARENA FOR THE TREE STRUCTURES, AND ANOTHER FOR ALL FILE PATHS

    // There's probably a way more efficient way to do this, but it's currently beyond
    // my skill and knowhow
    Arena arena_file_paths = {0};

    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_file_paths_buffer[arena_main_buffer_size];
    size_t arena_file_paths_buffer_length = sizeof(arena_file_paths_buffer) / sizeof(arena_file_paths_buffer[0]);
    arena_init(&arena_file_paths, arena_file_paths_buffer, arena_source_buffer_length);

    tree_copy_identical(&arena_file_paths, source_root, source_dir, target_root, target_dir);


    return 0;
}

void copyfile_testing(char *source, char *target) {
    copyfile(source, target, NULL, COPYFILE_DATA);
}
