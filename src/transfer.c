#include "../include/arena_tree.h"

void list_directories(char *file_path);
void print_arena_info(Arena *arena);

void tree_link_paths(Node *node, Arena *arena_temp, char *base_path) {
    // Base case
    if (node == NULL) return;

    char **full_path_array;
    full_path_array[0] = arena_string_duplicate(arena_temp, base_path, strlen(base_path)+1);
    full_path_array[1] = arena_string_duplicate(arena_temp, node->name, strlen(node->name)+1);
    size_t full_path_array_size = sizeof(full_path_array) / sizeof(full_path_array[0]);

    char *full_path = arena_string_concat(arena_temp, full_path_array, full_path_array_size);

    tree_link_paths(node->first_child, arena_temp, base_path);
    tree_link_paths(node->next_sibling, arena_temp, base_path);
}

int main(void) {
    Arena arena_source = {0};
    Arena arena_target = {0};
    Arena arena_temp = {0};

    // Define buffer size for the source and target arenas
    size_t arena_main_buffer_size = 262144;
    size_t arena_temp_buffer_size = PATH_MAX*64;
    // `_Alignas` ensures the buffer is aligned on creation
    _Alignas(DEFAULT_ALIGNMENT) byte arena_source_buffer[arena_main_buffer_size];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_target_buffer[arena_main_buffer_size];
    _Alignas(DEFAULT_ALIGNMENT) byte arena_temp_buffer[arena_temp_buffer_size];

    // Define buffer lengths for all arenas
    size_t arena_source_buffer_length = sizeof(arena_source_buffer) / sizeof(arena_source_buffer[0]);
    size_t arena_target_buffer_length = sizeof(arena_source_buffer) / sizeof(arena_source_buffer[0]);
    size_t arena_temp_buffer_length = sizeof(arena_temp_buffer) / sizeof(arena_temp_buffer[0]);

    // Initialize all arenas
    arena_init(&arena_source, arena_source_buffer, arena_source_buffer_length);
    arena_init(&arena_target, arena_target_buffer, arena_source_buffer_length);
    arena_init(&arena_temp, arena_temp_buffer, arena_temp_buffer_length);

    // TODO: Convert source directory to an array of directories (for much later)
    char *source_dir = "TestDirs/SmallDir";
    char *target_dir = "TestDirs/TargetSmallDirSame"; // Testing with identical directories as a start

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
    tree_fill(&arena_source, &arena_temp, source_root, source_dir);
    tree_fill(&arena_target, &arena_temp, target_root, target_dir);

    printf("SOURCE DIRECTORY TREE:\n");
    tree_print(source_root, 0);
    printf("\n");
    printf("TARGET DIRECTORY TREE:\n");
    tree_print(target_root, 0);
    printf("\n");

    bool is_identical = tree_is_identical(source_root->first_child, target_root->first_child);
    if (is_identical) printf("Trees are identical\n");
    else printf("Trees are not identical\n\n");

    // Before we can compare the trees, we need to figure out a way to link together
    // the file paths recursively
    // Problem is, I don't want to be recursing through the trees a total of what might be
    // like... 3-4 times. There's gotta be a better way....
    // Oh well, let's just get started on this and see where it goes
    // Actually (later), we can probably combine the file linking process with the tree
    // comparison process and just end that when the trees no longer match

    // First thing's first: we need a function for linking the file paths together
    // maybe this doesn't require recursion, but it most likely does
    printf("Source path links for %s\n", source_dir);
    size_t source_dir_len = strlen(source_dir);
    tree_link_paths(source_root, &arena_temp, source_dir);
    printf("\n");

    // printf("Target path links for %s\n", source_dir);
    // tree_link_paths(target_root);
    // printf("\n");

    // If the trees are identical, overwrite ONLY the files within each directory

    // If the trees are not identical, overwrite the ENTIRE tree

    // We can think about more elegant solutions when these two requirements are met

    return 0;
}

 void copyfile_testing(char *source, char *target) {
     copyfile(source, target, NULL, COPYFILE_DATA);
 }


// Rough draft function to be used later
// Gonna need this when comparing the files while running through
// the tree directories... but let's just focus on comparing the
// directories for now


void print_arena_info(Arena *arena) {
    printf("Main arena_source memory used: %zu/%zu bytes (%.2f%%)\n",
           arena->current_offset,
           arena->buffer_length,
           100.0 * (double)arena->current_offset / (double)arena->buffer_length);

    return;
}
