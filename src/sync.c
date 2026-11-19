#include "../include/sync.h"
#include "../include/arena.h"

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

void file_delete(const char *file_path) {
    if (remove(file_path) == 0) return;
    else printf("Error: Unable to delete file '%s'\n", file_path);
    return;
}

int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

//char *file_concat()

// void directory_delete()

 /*
    Visit each depth level of the source and target directory paths
    to ensure the target directory is synced with the source directory.
    The target directory is edited in-place and file_deletes/updates
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

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;
    while (source_ptr != NULL) {
        // BRAINSTORMING TRYING TO ACCOUNT FOR ALL INSTANCES:

        // SIMPLEST CASE: If the target directory is empty, copy over all source
        // directory files

        // If the source and target names are identical, check if the source
        // was edited more recently than the target
        // If it wasn't, obviously overwrite it. If it was, obviously continue
        // Ensure this only applies to files, not directories

        // If the source and target directory names are different before
        // reaching excess directories, I'm not sure what to do yet. Likely
        // just a simple delete

        // Delete extra directories and files found in the target directory that are not found in the source directory
        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            FTSENT *excess_file = target_ptr->fts_link;
            while (excess_file != NULL) {
                // If it's a file, delete it
                if ((excess_file->fts_info == FTS_F) || (excess_file->fts_info == FTS_D)) {
                    // MAKE THE RECURSIVE DELETION CHUNK INTO A FUNCTION
                    // MAKE THE STRING CONCATENATION INTO A FUNCTION
                    // Concatenate the directory name with it's path for passing to `file_delete`
                    size_t checkpoint = arena_checkpoint(arena_scratch);
                    char *full_path_array[3] = {excess_file->fts_path, "/", excess_file->fts_name};
                    char *full_path = arena_string_concat(arena_scratch, full_path_array, 3);
                    char *full_path_fts[] = {full_path, NULL};

                    // Delete the directory and everything under it using `fts_open`
                    FTS *directory_handle = fts_open(full_path_fts, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
                    if (directory_handle == NULL) {
                        perror("fts_open");
                        exit(EXIT_FAILURE);
                    }
                    FTSENT *entry;
                    while ((entry = fts_read(directory_handle)) != NULL) {
                        if (entry->fts_info != FTS_D) {
                            file_delete(entry->fts_path);
                        }
                    }
                    fts_close(directory_handle);
                    arena_rewind(arena_scratch, checkpoint);
                }
                excess_file = excess_file->fts_link;
            }
        }
        // Onto the source and target files
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
