#include "../include/sync.h"
#include "../include/arena.h"
#include <string.h>

// Create a NULL-terminated list of children files/directories for a given directory.
FTSENT *children_get_list(FTS *handle) {
    FTSENT *list_start = fts_read(handle);
    if (list_start == NULL) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }
    FTSENT *children_list = fts_children(handle, 0);

    return children_list;
}

// Pass to `fts_read()` to sort files alphabetically
int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

/*
Provide a scratch arena, base path, and file name to concatenate
the base path and file name to create a full path using the scratch arena.
Returns an arena checkpoint and concatenated file path.
It is recommended to reset the scratch arena to the returned checkpoint
after the file path is no longer needed (i.e., after using `directory_delete`)
 */
char *file_path_concatenate(Arena *arena_scratch, char *base_path, char *file_name) {
    char *file_name_array[3] = {base_path, "/", file_name};
    char *file_path = arena_string_concat(arena_scratch, file_name_array, 3);

    return file_path;
}

void file_copy(const char *source, const char *target) {
    if (copyfile(source, target, NULL, COPYFILE_DATA | COPYFILE_STAT) != 0) {
        perror("copyfile");
        exit(EXIT_FAILURE);
    }
    return;
}

// Delete a single file
void file_delete(char *file_path) {
    if (remove(file_path) != 0) {
        perror(file_path);
        exit(EXIT_FAILURE);
    }
    return;
}

// Recursively copy a directory and all of its contents. Overwrites if names are the same
void directory_copy(const char *source, const char *target) {
    copyfile_flags_t flags = COPYFILE_DATA | COPYFILE_STAT | COPYFILE_RECURSIVE;

    if (copyfile(source, target, NULL, flags) != 0) {
        perror("copyfile");
        exit(EXIT_FAILURE);
    }
    return;
}

// Recursively delete a directory and all of its contents
void directory_delete(char *file_path) {
    char *paths[2] = {file_path, NULL};
    FTS *handle = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (handle == NULL) {
        perror("directory_delete: fts_open");
        exit(EXIT_FAILURE);
    }

    FTSENT *entry;
    // Note: `fts_read` automatically recurses through the entire directory
    while ((entry = fts_read(handle)) != NULL) {
        // Skip directories to empty them before attempting to delete them,
        // since `remove` only works on empty directories
        // `fts_read` will recurse back to the empty directories and handle them
        if (entry->fts_info == FTS_D) continue;

        if (entry->fts_info == FTS_DNR || entry->fts_info == FTS_ERR || entry->fts_info == FTS_NS) {
            fprintf(stderr, "directory_delete: %s: %s\n", entry->fts_path, strerror(entry->fts_errno));
            exit(EXIT_FAILURE);
        }

        if (remove(entry->fts_path) != 0) {
            perror(entry->fts_path);
            exit(EXIT_FAILURE);
        }
    }
    fts_close(handle);
}

// Delete all Apple files from the directories (Temporary AI generated "bandaid" solution)
// TODO: Fully implement this into `directory_sync`. This should not be its own function.
// Allow `directory_sync` to handle this as it recurses through the file levels, since
// this approach adds an unnecessary extra recursion
static int is_junk(const char *name) {
    return strcmp(name, ".DS_Store") == 0 ||
           strcmp(name, ".Spotlight-V100") == 0 ||
           strcmp(name, ".fseventsd") == 0 ||
           strcmp(name, ".Trashes") == 0 ||
           strncmp(name, "._", 2) == 0;
}
void purge_junk(char *root) {
    char *paths[2] = {root, NULL};
    FTS *handle = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (handle == NULL) {
        perror("purge_junk: fts_open");
        exit(EXIT_FAILURE);
    }
    FTSENT *entry;
    while ((entry = fts_read(handle)) != NULL) {
        if (entry->fts_level == 0 || !is_junk(entry->fts_name)) continue;

        if (entry->fts_info == FTS_D) {
            fts_set(handle, entry, FTS_SKIP);   // don't descend, we're deleting it
            directory_delete(entry->fts_path);
        } else if (entry->fts_info != FTS_DP) {
            file_delete(entry->fts_path);
        }
    }
    fts_close(handle);
}

 /*
Visit each depth level of the source and target directory paths
to ensure the target directory is synced with the source directory.
The target directory is edited in-place and deletes/updates
files according to the source directory's file hierarchy structure.
A scratch arena must be provided for string operations.
*/
void sync_directories(Arena *arena_scratch, char *source, char *target) {
    printf("Syncing source folder '%s' with '%s'", source, target);
    // Delete unneeded Apple files
    purge_junk(source);
    purge_junk(target);

    // Open the source and target directories while sorting them `by_name`
    // TODO: Need more verbose error handling and messaging here
    char *source_fts[2] = {source, NULL};
    char *target_fts[2] = {target, NULL};
    FTS *source_handle = fts_open(source_fts, FTS_PHYSICAL, by_name);
    if (source_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }
    FTS *target_handle = fts_open(target_fts, FTS_PHYSICAL, by_name);
    if (target_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // THIS PRINT STATEMENT WILL BE INCORRECT WHEN THE
    // Create a linked list of all files/directories in the current directory depth level
    FTSENT *source_children_list = children_get_list(source_handle);
    if (source_children_list == NULL) {
        printf("Error: An empty source directory was provided\n");
        return;
    }

    // IMPORTANT NOTE: Doing the empty directory check properly would require some research on
    // error handling. So, we're gonna do it the incorrect way for now and just
    // treat NULL as an empty directory (likely an unsafe practice)
    FTSENT *target_children_list = children_get_list(target_handle);
    if (target_children_list == NULL) {
        printf("Copying source directory '%s` to empty target directory '%s'...\n", source, target);
        size_t checkpoint = arena_checkpoint(arena_scratch);
        char *source_array[2] = {source, "/"};
        char *source_slash = arena_string_concat(arena_scratch, source_array, 2);

        directory_copy(source_slash, target);
        arena_rewind(arena_scratch, checkpoint);
        return;
    }

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;

    while (source_ptr != NULL) {
        size_t checkpoint = arena_checkpoint(arena_scratch); // Checkpoint for resetting arena between while loops

        // The plan:
        // Compare file name, edit time, and sizes - note that directory edit times only change when they're renamed. The same doesn't apply to files
        // (double check if the edit time/modification date claim is true)

        //// DIRECTORY LOGIC ////
        // If the source and target have the same name...
        if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) == 0) &&
            // and both are directories...
            (source_ptr->fts_info == FTS_D) && (target_ptr->fts_info == FTS_D) &&
            // and the source was edited more recently than the target
            (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime)) {
                // overwrite the source directory into the target directory
                char *source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                printf("Overwriting directory '%s' to directory '%s\n", source_full_path, target_full_path);
                directory_copy(source_full_path, target_full_path);
        }
        // If the target directory's name doesn't match the source directory's name, delete the target directory
        if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) != 0) &&
            (source_ptr->fts_info == FTS_D) && (target_ptr->fts_info == FTS_D)) {
            char *full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
            printf("Normal directory delete for directory: '%s'\n\n", full_path);
            directory_delete(full_path);
        }


        //// FILE LOGIC ////
        // If the source and target have the same name...
        if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) == 0) &&
            // and both are files...
            (source_ptr->fts_info == FTS_F) && (target_ptr->fts_info == FTS_F) &&
            // and the source was edited more recently than the target
            (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime)) {
                // overwrite the source file into the target file
                char *source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                printf("Overwriting file '%s' to file '%s\n", source_full_path, target_full_path);
                file_copy(source_full_path, target_full_path);
        }
        // If the target file's name doesn't match the source file's name, delete the target file
        if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) != 0) &&
            (source_ptr->fts_info == FTS_F) && (target_ptr->fts_info == FTS_F)) {
            char *full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
            printf("Normal file delete for file: '%s'\n\n", full_path);
            file_delete(full_path);
        }


        // If the source and target directory names are different before
        // reaching excess directories, I'm not sure what to do yet. Likely
        // just a simple delete
        // The prior logic also might ensure that doesn't happen

        //// EXTRA FILE LOGIC ////
        // TODO: ADD LOGIC FOR IF THE SOURCE DIRECTORY IS LONGER THAN THE TARGET
        // this should follow the below logic
        // Delete extra directories and files found in the target directory that are not found in the source directory
        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            FTSENT *excess_file = target_ptr->fts_link;
            while (excess_file != NULL) {
                // Create a full file path and arena checkpoint
                char *excess_file_path = file_path_concatenate(arena_scratch, excess_file->fts_path, excess_file->fts_name);
                // If it's a file, just delete the file
                if (excess_file->fts_info == FTS_F) {
                    printf("Excess file delete for file: %s\n", excess_file_path);
                    file_delete(excess_file_path);
                }
                // Otherwise, if it's a directory, recursively delete the directory
                else if (excess_file->fts_info == FTS_D) {
                    printf("Excess directory delete for directory: %s\n", excess_file_path);
                    directory_delete(excess_file_path);
                }
                // Otherwise, we don't know what the heck we're dealing with
                // (this error handling could almost definitely be improved
                // using the `stat` struct. Let's just get things moving for now)
                else {
                    printf("ERROR: Unrecognized file type for file: '%s'\n", excess_file_path);
                    return;
                }
                excess_file = excess_file->fts_link;
            }
        }
        // Move to the next source and target files
        source_ptr = source_ptr->fts_link;
        target_ptr = target_ptr->fts_link;
        // When you add recursion: create a function that returns the next directory
        // to be passed here. Then call this function in the recursion call
        arena_rewind(arena_scratch, checkpoint);
    }

    fts_close(source_handle);
    fts_close(target_handle);
}
