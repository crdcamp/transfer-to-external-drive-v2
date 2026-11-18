#include "../include/compare.h"
#include "../include/arena.h"

void children_print_list(FTSENT *children_list) {
    FTSENT *ptr = children_list;
    printf("Parent: %s\n", ptr->fts_parent->fts_name);
    printf("Depth: %i\n", ptr->fts_level);
    printf("Files: ");
    while (ptr != NULL) {
        printf("%s ", ptr->fts_name);
        ptr = ptr->fts_link;
    }
    printf("\n");
}

FTSENT *children_get_list(FTS *handle) {
    FTSENT *list_start = fts_read(handle);
    if (list_start == NULL) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }
    FTSENT *children_list = fts_children(handle, 0);
    if (children_list == NULL) {
        perror("fts_children");
        exit(EXIT_FAILURE);
    }
    return children_list;
}

void delete(const char *file_path) {
    if (remove(file_path) == 0) {
        printf("File '%s' deleted\n", file_path);
    }
    else {
        printf("Error: Unable to delete file '%s'\n", file_path);
    }
    return;
}

static int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

 /*
    Visit each depth level of the source and target directory paths
    to ensure the target directory mirrors the source directory.
    The target directory is edited in-place and deletes/updates
    files according to the source directory's file hierarchy structure.
    A scratch `Arena` must be provided for string operations.
*/
void compare_directories(Arena *arena_scratch, char **source, char **target) {
    // Open the source and target directories while sorting them `by_name`
    FTS *source_handle = fts_open(source, FTS_PHYSICAL, by_name);
    if (source_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }
    FTS *target_handle = fts_open(target, FTS_PHYSICAL, by_name);
    if (target_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Create a linked list of all files/directories in the current directory
    FTSENT *source_children_list = children_get_list(source_handle);
    FTSENT *target_children_list = children_get_list(target_handle);

    // Test print
    // children_print_list(source_children_list);
    // printf("\n");

    // children_print_list(target_children_list);
    // printf("\n");

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;
    while (source_ptr != NULL) {
        printf("Current source file: %s\n", source_ptr->fts_name);
        printf("Current target file: %s\n", target_ptr->fts_name);
        printf("\n");

        // Note that the following condition is probably the last,
        // or one of the last conditions you should be checking for,
        // meaning that the rest will go above this one

        // If the target directory's current depth has more files than
        // the source directory, delete the extraneous files
        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            struct _ftsent *next_file = target_ptr->fts_link;
            printf("Next target file: %s\n", next_file->fts_name);

            // If it's a file, just delete the file
            if (next_file->fts_info == FTS_F) {
                // Concatenate `fts_path` and `fts_name`
                size_t checkpoint = arena_checkpoint(arena_scratch);

                char *full_path_array[3] = {next_file->fts_path, "/", next_file->fts_name};
                char *full_path = arena_string_concat(arena_scratch, full_path_array, 3);
                printf("File %s is a file\n", next_file->fts_path);
                printf("Attempting to delete file: %s\n", next_file->fts_path);
                delete(full_path);

                arena_rewind(arena_scratch, checkpoint);
            }
            // If it's a directory, recursively delete the directory
            if (next_file->fts_info == FTS_D) {
                printf("File %s is a directory\n", target_ptr->fts_accpath);
            }
            // If it's a directory, recursively delete the director
        }
        source_ptr = source_ptr->fts_link;
        target_ptr = target_ptr->fts_link;
    }

    // Now, how do we set this recursion up so it continues down
    // the rest of the file paths?
    // I think this would involve passing the file paths that are directories
    // with an additional slash in them to access the next level.
    // This could also then be used recursively. You also already have
    // and arena function to accomplish this
    // Hopefully this recursion actually reaches all tree paths when implemented...
}
