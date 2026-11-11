#include <limits.h>

#include "../include/arena_tree.h"

void list_directories(char *file_path);

int main(void) {
    Arena arena = {0};
    Arena arena_temp = {0};

    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_buffer[262144];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[PATH_MAX * 64];

    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);
    size_t arena_temp_buffer_length = sizeof(arena_temp_buffer) / sizeof(arena_temp_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&arena_temp, arena_temp_buffer, arena_temp_buffer_length);

    // We're just testing a smaller file structure for now
    char *dir_source = "TestDirs/Code";
    char *dir_target = "TestDirs/Target";

    // Initialize the root_dir_source for the tree structure to be stored in the arena
    Node *root_dir_source = new_node(&arena, dir_source);
    if (root_dir_source == NULL) return 1;

    // Create a directory tree to be stored in the arena
    tree_fill(&arena, &arena_temp, root_dir_source, dir_source);
    printf("Main arena memory used: %zu/%zu bytes (%.2f%%)\n",
           arena.current_offset,
           arena.buffer_length,
           100.0 * (double)arena.current_offset / (double)arena.buffer_length);
    //tree_print(root_dir_source, 0);

    list_directories("TestDirs/SmallDir/SmallSubDir");

    return 0;
}

void list_directories(char *file_path) {
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
            printf("File: '%s\n", entry->d_name);
        }
    }
}
