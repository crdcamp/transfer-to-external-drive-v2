#include "../include/sync.h"
#include "../include/arena.h"
#include <stdint.h>
#include <stdio.h>

/*
  Create a NULL-terminated list of children files/directories for a given directory.
 */
FTSENT *children_get_list(FTS *handle) {
    FTSENT *list_start = fts_read(handle);
    if (list_start == NULL) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }
    FTSENT *children_list = fts_children(handle, 0);

    return children_list;
}

/*
   Pass to `fts_read()` to sort files alphabetically
*/
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
ConcatResult file_path_concatenate(Arena *arena_scratch, char *base_path, char *file_name) {
    ConcatResult result = {0};
    result.arena_checkpoint = arena_checkpoint(arena_scratch);

    char *file_name_array[3] = {base_path, "/", file_name};
    result.file_path = arena_string_concat(arena_scratch, file_name_array, 3);

    return result;
}

void file_delete(char *file_path) {
    if (remove(file_path) != 0) {
        perror(file_path);
        exit(EXIT_FAILURE);
    }
}

/*
  Given a path to a file or directory, delete the file, or the entire directory
*/
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

 /*
    Visit each depth level of the source and target directory paths
    to ensure the target directory is synced with the source directory.
    The target directory is edited in-place and deletes/updates
    files according to the source directory's file hierarchy structure.
    A scratch arena must be provided for string operations.
*/
// TODO: Simplify arena rewinding. It should ideally be called just once
// You're partially defeating the purpose of an arena here
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

    // NOTE: Doing the empty directory check properly would require some research on
    // error handling. So, we're gonna do it the incorrect way for now and just
    // treat NULL as an empty directory

    // Create a linked list of all files/directories in the current directory depth level
    FTSENT *source_children_list = children_get_list(source_handle);
    if (source_children_list == NULL) {
        printf("Error: An empty source directory was provided\n");
        return;
    }

    // If `target_children_list` is NULL, then we (probably) have an empty directory,
    // so we need to copy the entire source
    // WARNING: I have an intuition that this is an unsafe way to handle this
    FTSENT *target_children_list = children_get_list(target_handle);
    if (target_children_list == NULL) {
        printf("target_children_list is NULL\n");
        return;
    }

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;

    while (source_ptr != NULL) {
        // BRAINSTORMING TRYING TO ACCOUNT FOR ALL INSTANCES:
        ConcatResult source_file_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
        ConcatResult target_file_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);

        // If the files match, move onto the next file
        if (source_ptr->fts_name == target_ptr->fts_name) {
            printf("Source file '%s' matches target file '%s'. Moving on\n", source_ptr->fts_name, target_ptr->fts_name);
            source_ptr = source_ptr->fts_link;
            target_ptr = target_ptr->fts_link;
        }

        // If the current target directory depth doesn't match the source, delete it
        // NOTE: Bit heavy handed, but a more efficient approach requires way more tedious effort
        // We can implement a more advanced version of this project next time I have the time to do so
        // Delete non-matching files
        if ((source_ptr->fts_name != target_ptr->fts_name) && (target_ptr->fts_info == FTS_F)) {
            printf("Normal file delete for file `%s`. Full path: %s\n", target_ptr->fts_name, target_file_path.file_path);
            file_delete(target_file_path.file_path);
        }
        // Delete non-matching directories
        if ((source_ptr->fts_path != target_ptr->fts_path) && (target_ptr->fts_info == FTS_D)) {
            printf("Normal directory delete for directory '%s`. Full path: %s\n", target_ptr->fts_name, target_file_path.file_path);
            directory_delete(target_file_path.file_path);
        }

        // Maybe there's a way to determine if a folder was renamed and that could
        // also be incorporated somehow?
        // Could maybe use the ID field in the stat struct? Could pretty easily
        // test that by just renaming a file
        // Note that there's also a `rename` function if you can sort this process out

        // If the source and target names are identical, check if the source
        // was edited more recently than the target
        // If it wasn't, obviously overwrite it. If it was, obviously continue
        // Ensure this only applies to files, not directories

        // Consider keeping some metadata on directories of every level to see
        // if subdirectories have the same name. If they do, check that the subdirectory
        // Matches the saved directory and move it to the location if it matches
        // (I'd consider maybe not doing this at all, but there has to be some
        // clever way to move directories instead of deleting and remaking them, right?)

        // If the source and target directory names are different before
        // reaching excess directories, I'm not sure what to do yet. Likely
        // just a simple delete

        // Delete extra directories and files found in the target directory that are not found in the source directory
        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            FTSENT *excess_file = target_ptr->fts_link;
            while (excess_file != NULL) {
                // Create a full file path and arena checkpoint
                ConcatResult excess_file_path = file_path_concatenate(arena_scratch, excess_file->fts_path, excess_file->fts_name);
                // If it's a file, just delete the file
                if (excess_file->fts_info == FTS_F) {
                    printf("Excess file delete for file: %s\n", excess_file_path.file_path);
                    file_delete(excess_file_path.file_path);
                }
                // Otherwise, if it's a directory, recursively delete the directory
                else if (excess_file->fts_info == FTS_D) {
                    printf("Excess directory delete for directory: %s\n", excess_file_path.file_path);
                    directory_delete(excess_file_path.file_path);
                }
                // Otherwise, we don't know what the heck we're dealing with
                // (this error handling could almost definitely be improved
                // using the `stat` struct. Let's just get things moving for now)
                else {
                    printf("ERROR: Unrecognized file type for file: '%s'. Returning...\n", excess_file_path.file_path);
                    arena_rewind(arena_scratch, excess_file_path.arena_checkpoint);
                    return;
                }
                excess_file = excess_file->fts_link;
                arena_rewind(arena_scratch, excess_file_path.arena_checkpoint);
            }
        }
    // Move to the next source and target files
    source_ptr = source_ptr->fts_link;
    target_ptr = target_ptr->fts_link;
    }

    // // Now, how do we set this recursion up so it continues down
    // // the rest of the file paths?
    // // I think this would involve passing the file paths that are directories
    // // with an additional slash in them to access the next level.
    // // This could also then be used recursively. You also already have
    // // and arena function to accomplish this
    // // Hopefully this recursion actually reaches all tree paths when implemented...

    // fts_close(source_handle);
    // fts_close(target_handle);
}
