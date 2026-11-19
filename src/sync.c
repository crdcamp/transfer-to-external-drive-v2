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

void delete_file(const char *file_path) {
    if (remove(file_path) == 0) {
    }
    else {
        printf("Error: Unable to delete file '%s'\n", file_path);
    }
    return;
}

int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

 /*
    Visit each depth level of the source and target directory paths
    to ensure the target directory mirrors the source directory.
    The target directory is edited in-place and delete_files/updates
    files according to the source directory's file hierarchy structure.
    A scratch `Arena` must be provided for string operations.
*/
void sync_directories(Arena *arena_scratch, char **source, char **target) {
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

        // Delete extra directories and files found in the target directory that are not found in the source directory
        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            FTSENT *excess_file = target_ptr->fts_link;
            while (excess_file != NULL) {
                // If it's a file, delete it
                if (excess_file->fts_info == FTS_F) {
                    // Concatenate the file name with it's path for passing to `delete_file`
                    size_t checkpoint = arena_checkpoint(arena_scratch); // Create a checkpoint for the scratch arena reset later
                    char *full_path_array[3] = {excess_file->fts_path, "/", excess_file->fts_name};
                    char *full_path = arena_string_concat(arena_scratch, full_path_array, 3);

                    // Delete the file
                    delete_file(full_path);
                    arena_rewind(arena_scratch, checkpoint); // Reset the scratch arena
                }

                // If it's a directory, recursively delete_file the directory
                else if (excess_file->fts_info == FTS_D) {
                    printf("Excess file '%s' is a directory\n", excess_file->fts_accpath);
                    size_t checkpoint = arena_checkpoint(arena_scratch);
                    char *full_path_array[3] = {excess_file->fts_path};
                }
                // If it's a directory, recursively delete_file the director
                // DON'T FORGET to close the file if you end up using fts_open()
                excess_file = excess_file->fts_link;
            }
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

    fts_close(source_handle);
    fts_close(target_handle);
}
