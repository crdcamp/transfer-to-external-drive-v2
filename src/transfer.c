#include <limits.h>

#include "../include/arena_tree.h"

void list_directories(char *file_path);
void print_arena_info(Arena *arena);

int main(void) {
    Arena arena_source = {0};
    Arena arena_target = {0};
    Arena arena_temp = {0};

    // Define buffer size for the source and target arenas
    size_t arena_main_buffer_size = 262144;
    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_source_buffer[arena_main_buffer_size];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_target_buffer[arena_main_buffer_size];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[PATH_MAX * 64];

    // Define buffer lengths for all arenas
    size_t arena_source_buffer_length = sizeof(arena_source_buffer) / sizeof(arena_source_buffer[0]);
    size_t arena_target_buffer_length = sizeof(arena_source_buffer) / sizeof(arena_source_buffer[0]);
    size_t arena_temp_buffer_length = sizeof(arena_temp_buffer) / sizeof(arena_temp_buffer[0]);

    // Initialize all arenas
    arena_init(&arena_source, arena_source_buffer, arena_source_buffer_length);
    arena_init(&arena_target, arena_target_buffer, arena_source_buffer_length);
    arena_init(&arena_temp, arena_temp_buffer, arena_temp_buffer_length);

    // TODO: Convert source directory to an array of directories (for much later)
    char *dir_source = "TestDirs/SmallDir";
    char *dir_target = "TestDirs/TargetSmallDirDifferent"; // Testing with identical directories as a start

    // Initialize the source and target directory trees
    Node *root_dir_source = new_node(&arena_source, dir_source);
    if (root_dir_source == NULL) return 1;
    Node *root_dir_target = new_node(&arena_target, dir_target);
    if (root_dir_target == NULL) return 1;

    // Fill trees for the source and target directories
    tree_fill(&arena_source, &arena_temp, root_dir_source, dir_source);
    tree_fill(&arena_target, &arena_temp, root_dir_target, dir_target);

    printf("SOURCE DIRECTORY TREE:\n");
    tree_print(root_dir_source, 0);
    printf("\n");
    printf("TARGET DIRECTORY TREE:\n");
    tree_print(root_dir_target, 0);
    printf("\n");

    bool result = tree_is_identical(root_dir_source->first_child, root_dir_target->first_child);
    printf("Tree comparison result: %d\n", result);

    return 0;
}

// Rough draft function to be used later
// Gonna need this when comparing the files while running through
// the tree directories... but let's just focus on comparing the
// directories for now
void list_directories(char *file_path) {
    printf("Files in direcroty: %s\n", file_path);
    DIR *dirp = opendir(file_path);
    if (dirp == NULL) {
        printf("Failed opening directory: %s\n", file_path);
        perror("'opendir' failed");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_REG) {
            printf("%s\n", entry->d_name);
        }
    }
}

void print_arena_info(Arena *arena) {
    printf("Main arena_source memory used: %zu/%zu bytes (%.2f%%)\n",
           arena->current_offset,
           arena->buffer_length,
           100.0 * (double)arena->current_offset / (double)arena->buffer_length);

    return;
}
