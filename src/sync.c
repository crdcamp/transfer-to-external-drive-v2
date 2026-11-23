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
It is recommended to reset the scratch arena after the file path is no longer needed.
*/
char *file_path_concatenate(Arena *arena_scratch, char *base_path, char *file_name) {
    char *file_name_array[3] = {base_path, "/", file_name};
    char *file_path = arena_string_concat(arena_scratch, file_name_array, 3);

    return file_path;
}

// Copy a single file
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
        // Deliberately didn`t call `file_delete` here to keep things separate. Ya neva know...
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
            fts_set(handle, entry, FTS_SKIP);   // don`t descend, we`re deleting it
            directory_delete(entry->fts_path);
        } else if (entry->fts_info != FTS_DP) {
            file_delete(entry->fts_path);
        }
    }
    fts_close(handle);
}

// Given an fts entry, determine if the file is a directory
bool is_a_directory(FTSENT *entry) {
    if (entry->fts_info == FTS_D) {
        return true;
    } else return false;
}

// Given an fts entry, determine if the file is a file
bool is_a_file(FTSENT *entry) {
    if (entry->fts_info == FTS_F) {
        return true;
    } else return false;
}

/*
Visit each depth level of the source and target directory paths
to ensure the target directory is synced with the source directory.
The target directory is edited in-place and deletes/updates
files according to the source directory`s file hierarchy structure.
A scratch arena must be provided for string operations.
*/
// TODO: Double check all return statements for arena rewinds and file closes
void sync_directories(Arena *arena_scratch, char *source, char *target) {
    size_t checkpoint = arena_checkpoint(arena_scratch); // Checkpoint for resetting arena between recursions
    printf("Syncing source directory `%s` with target directory `%s`\n", source, target);
    // DON`T FORGET TO ADD BASE CASE HERE WHEN IMPLEMENTING RECURSION

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

    // NOTE: THESE NULL CHECKS MIGHT CAUSE ISSUES WHEN ACTUALLY RECURSING
    // YOU NEED A BASE CASE DON`T FORGET
    // REVISE THIS PRINT STATEMENT. It will convey incorrect information when recursing has been implemented
    // Create a linked list of all files/directories in the current directory depth level
    FTSENT *source_children_list = children_get_list(source_handle);
    if (source_children_list == NULL) {

        printf("Error: An empty source directory was provided\n");

        fts_close(source_handle);
        fts_close(target_handle);
        return;
    }

    // IMPORTANT NOTE: Doing the empty directory check properly would require some research on
    // error handling. So, we`re gonna do it the incorrect way for now and just
    // treat NULL as an empty directory (likely an unsafe practice)
    FTSENT *target_children_list = children_get_list(target_handle);
    if (target_children_list == NULL) {

        printf("Copying source directory `%s` to empty target directory `%s`...\n", source, target);

        char *source_array[2] = {source, "/"};
        char *source_slash = arena_string_concat(arena_scratch, source_array, 2);
        directory_copy(source_slash, target);

        arena_rewind(arena_scratch, checkpoint);
        fts_close(source_handle);
        fts_close(target_handle);
        return;
    }

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;
    while (source_ptr != NULL || target_ptr != NULL) {
        // If `target_ptr` is NULL, that means all the remaining source files are not in the target directory
        // So, we copy the rest of the files in the current source directory to the target directory
        // Advance only `source_ptr`
        if (target_ptr == NULL) {
            while (source_ptr != NULL) {
                char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);

                if (is_a_directory(source_ptr)) {
                    printf("Copying source DIRECTORY '%s' to target '%s'\n", source_path, target_path);
                    directory_copy(source_path, target_path);
                }
                else if (is_a_file(source_ptr)) {
                    printf("Copying source FILE '%s' to target '%s'\n", source_path, target_path);
                    file_copy(source_path, target_path);
                }
                source_ptr = source_ptr->fts_link;
            }
            continue;
        }
        // If `source_ptr` is NULL, that means all remaining target files are not in the source directory
        // So, we delete the rest of the files in the current target directory
        // Advance only `target_ptr`
        if (source_ptr == NULL) {
            while (target_ptr != NULL) {
                char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                if (is_a_directory(target_ptr)) {
                    printf("Deleting DIRECTORY `%s`\n", target_path);
                    directory_delete(target_path);
                }
                else if (is_a_file(target_ptr)) {
                    printf("Deleting FILE `%s`\n", target_path);
                    file_delete(target_path);
                }
                target_ptr = target_ptr->fts_link;
            }
            continue;
        }

        // Because there are no duplicates and the files are sorted alphabetically, we can use string comparisons here
        int string_comparison = strcmp(source_ptr->fts_name, target_ptr->fts_name);

        // If the file names are the same and...
        if (string_comparison == 0) {
            // both are directories, advance both pointers (leave further edits to the next recursion)
            if (is_a_directory(source_ptr) && is_a_directory(target_ptr)) {
                char *source_subdir = file_path_concatenate(arena_scratch, source, source_ptr->fts_name);
                char *target_subdir = file_path_concatenate(arena_scratch, target, target_ptr->fts_name);
                sync_directories(arena_scratch, source_subdir, target_subdir);

                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // both are files and they have the same last modification time, advance both pointers
            if (is_a_file(source_ptr) && is_a_file(target_ptr) &&
                (source_ptr->fts_statp->st_mtime == target_ptr->fts_statp->st_mtime)) {
                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // both are files and the source file was edited more recently than the target file,
            // or the target file was edited more recently than the source,
            // overwrite the source file to the target regardless if the target was edited more recently
            if (is_a_file(source_ptr) && is_a_file(target_ptr) &&
                (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime) ||
                    (source_ptr->fts_statp->st_mtime < target_ptr->fts_statp->st_mtime)) {
                char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);

                file_copy(source_path, target_path);
                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // the source file is a file and the target file is a directory,
            // delete the target directory and copy the source file
            if (is_a_file(source_ptr) && is_a_directory(target_ptr)) {
                char *target_delete_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                directory_delete(target_delete_path);

                char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, source_ptr->fts_name);
                file_copy(source_path, target_path);

                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // the source file is a directory and the target file is a file,
            // delete the target file and copy the source directory
            if (is_a_directory(source_ptr) && is_a_file(target_ptr)) {
                char *target_delete_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                file_delete(target_delete_path);

                char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, source_ptr->fts_name);
                directory_copy(source_path, target_path);

                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // either file has an unsupported type
            if (!is_a_directory(source_ptr) || !is_a_directory(target_ptr) ||
                !is_a_file(source_ptr) || !is_a_file(target_ptr)) {
                char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                fprintf(stderr,
                        "sync_directories: cannot sync `%s` with `%s`: unsupported file type "
                        "(source fts_info = %d, target fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        source_path, target_path, source_ptr->fts_info, target_ptr->fts_info);
                exit(EXIT_FAILURE);
                }
        }

        // cmp < 0: the source name comes earlier alphabetically
        // cmp > 0: the source name comes later alphabetically
        // If `string_comparison` < 0, that means the file is in the source, and not the target
        // So, we copy the source to the target
        // Advance only `source_ptr`
        if (string_comparison < 0) {
            char *source_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
            if (is_a_directory(source_ptr)) {
                char *target_path = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);
                directory_copy(source_path, target_path);
            }
            else if (is_a_file(source_ptr)) {
                char *target_path = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);
                file_copy(source_path, target_path);
            }
            else {
                fprintf(stderr,
                        "sync_directories: cannot sync `%s`: unsupported file type (fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        source_path, source_ptr->fts_info);
                exit(EXIT_FAILURE);
            }
            source_ptr = source_ptr->fts_link;
            continue;
        }

        // If `string_comparison` > 0 and they're the same file type, that means the file is in the target, and not the source
        // So, we delete the target file
        // Advance only `target_ptr`
        if (string_comparison > 0) {
            char *target_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
            if (is_a_directory(target_ptr)) {
                directory_delete(target_path);
            }
            else if (is_a_file(target_ptr)) {
                file_delete(target_path);
            }
            else {
                fprintf(stderr,
                        "sync_directories: cannot sync `%s`: unsupported file type (fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        target_path, target_ptr->fts_info);
                exit(EXIT_FAILURE);
            }
            target_ptr = target_ptr->fts_link;
            continue;
        }
    }

    // NOTE: When you add recursion: create a function that returns a pointer to the next directory string
    // that will be passed here. Then call this function in the recursion call
    arena_rewind(arena_scratch, checkpoint);
    fts_close(source_handle);
    fts_close(target_handle);
}
