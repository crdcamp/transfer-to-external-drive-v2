#include "../include/sync.h"
#include "../include/arena.h"
#include <fts.h>
#include <stdbool.h>
#include <string.h>

// Pass to `fts_open()` to sort files alphabetically
int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

// Create a NULL-terminated list of children files/directories for a given directory.
FTSENT *children_get_list(FTS *handle) {
    errno = 0; // Always set to 0 in a function, since errno can be edited by other operations
    // If `fts_info` returns as FTS_DNR, FTS_ERR or FTS_NS, that will set the external `errno`
    FTSENT *list_head = fts_read(handle); // Handle has the be read before being passed to `fts_children`
    if (errno != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    FTSENT *children_list = fts_children(handle, 0);
    if (errno != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return children_list;
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
        printf("Error: %s\n", strerror(errno));
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
        printf("Error: %s\n", strerror(errno));
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

// TODO: Find a way to avoid redeclaring these every time the function is called
// In other words, store them in an arena
FTSENT *skip_apple_files(FTSENT *entry) {
    char *apple_files[] = {".DS_Store", ".Spotlight-V100", ".fseventsd", ".Trashes"};
    size_t apple_files_size = sizeof(apple_files) / sizeof(apple_files[0]);

    while (entry != NULL) {
        bool is_apple = false;
        for (size_t i = 0; i < apple_files_size; i++) {
            if (strcmp(entry->fts_name, apple_files[i]) == 0) {
                printf("Apple file `%s` found. Skipping\n\n", apple_files[i]);
                is_apple = true;
                break;
            }
        }
        if (!is_apple) break;
        entry = entry->fts_link;
    }
    return entry;
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
void directories_sync(Arena *arena_scratch, char *source, char *target) {
    size_t checkpoint = arena_checkpoint(arena_scratch); // Checkpoint for resetting arena between recursions
    printf("Syncing source directory `%s` with target directory `%s`\n", source, target);



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
                char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);

                if (is_a_directory(source_ptr)) {
                    printf("Copying source DIRECTORY '%s' to target '%s'\n", source_path_copy, target_path_copy);
                    directory_copy(source_path_copy, target_path_copy);
                }
                else if (is_a_file(source_ptr)) {
                    printf("Copying source FILE '%s' to target '%s'\n", source_path_copy, target_path_copy);
                    file_copy(source_path_copy, target_path_copy);
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
                char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                if (is_a_directory(target_ptr)) {
                    printf("Deleting DIRECTORY `%s`\n", target_path_copy);
                    directory_delete(target_path_copy);
                }
                else if (is_a_file(target_ptr)) {
                    printf("Deleting FILE `%s`\n", target_path_copy);
                    file_delete(target_path_copy);
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
            // Both are directories: recurse into them
            if (is_a_directory(source_ptr) && is_a_directory(target_ptr)) {
                directories_sync(arena_scratch, source_ptr->fts_path, target_ptr->fts_path);

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
                char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);

                file_copy(source_path_copy, target_path_copy);
                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // the source file is a file and the target file is a directory,
            // delete the target directory and copy the source file
            if (is_a_file(source_ptr) && is_a_directory(target_ptr)) {
                char *target_delete_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                directory_delete(target_delete_path);

                char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, source_ptr->fts_name);
                file_copy(source_path_copy, target_path_copy);

                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // the source file is a directory and the target file is a file,
            // delete the target file and copy the source directory
            if (is_a_directory(source_ptr) && is_a_file(target_ptr)) {
                char *target_delete_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                file_delete(target_delete_path);

                char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, source_ptr->fts_name);
                directory_copy(source_path_copy, target_path_copy);

                source_ptr = source_ptr->fts_link;
                target_ptr = target_ptr->fts_link;
                continue;
            }
            // either file has an unsupported type
            if (!is_a_directory(source_ptr) || !is_a_directory(target_ptr) ||
                !is_a_file(source_ptr) || !is_a_file(target_ptr)) {
                char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                fprintf(stderr,
                        "directories_sync: cannot sync `%s` with `%s`: unsupported file type "
                        "(source fts_info = %d, target fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        source_path_copy, target_path_copy, source_ptr->fts_info, target_ptr->fts_info);
                exit(EXIT_FAILURE);
                }
        }

        // cmp < 0: the source name comes earlier alphabetically
        // cmp > 0: the source name comes later alphabetically
        // If `string_comparison` < 0, that means the file is in the source, and not the target
        // So, we copy the source to the target
        // Advance only `source_ptr`
        if (string_comparison < 0) {
            char *source_path_copy = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
            if (is_a_directory(source_ptr)) {
                char *target_path_copy = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);
                directory_copy(source_path_copy, target_path_copy);
            }
            else if (is_a_file(source_ptr)) {
                char *target_path_copy = file_path_concatenate(arena_scratch, target, source_ptr->fts_name);
                file_copy(source_path_copy, target_path_copy);
            }
            else {
                fprintf(stderr,
                        "directories_sync: cannot sync `%s`: unsupported file type (fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        source_path_copy, source_ptr->fts_info);
                exit(EXIT_FAILURE);
            }
            source_ptr = source_ptr->fts_link;
            continue;
        }

        // If `string_comparison` > 0 and they're the same file type, that means the file is in the target, and not the source
        // So, we delete the target file
        // Advance only `target_ptr`
        if (string_comparison > 0) {
            char *target_path_copy = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
            if (is_a_directory(target_ptr)) {
                directory_delete(target_path_copy);
            }
            else if (is_a_file(target_ptr)) {
                file_delete(target_path_copy);
            }
            else {
                fprintf(stderr,
                        "directories_sync: cannot sync `%s`: unsupported file type (fts_info = %d). "
                        "Only regular files and directories are supported (symlinks, sockets, FIFOs, and devices are not).\n",
                        target_path_copy, target_ptr->fts_info);
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





///////////////////////////
///// REWRITE SECTION /////
///////////////////////////
FTSENT *test_children_get_list(FTS *handle) { // Assumes `fts_open` was used prior
    errno = 0; // Always set to 0 in a function, since errno can be edited by other operations
    FTSENT *list_head = fts_read(handle); // Handle has the be read before being passed to `fts_children`
    if (errno != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    FTSENT *children_list = fts_children(handle, 0);
    // If `fts_info` returns as FTS_DNR, FTS_ERR or FTS_NS, that will set the external `errno`
    if (errno != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return children_list;
}

// Copy a single file
void test_file_copy(const char *source, const char *target) {
    if (copyfile(source, target, NULL, COPYFILE_DATA | COPYFILE_STAT) != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

// Copy a directory
void test_directory_copy(const char *source, const char *target) {
    copyfile_flags_t flags = COPYFILE_DATA | COPYFILE_STAT | COPYFILE_RECURSIVE;
    if (copyfile(source, target, NULL, flags) != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

// TODO: Change if statements to switch cases
// TODO: Implement a directory skip after null checks
// TODO: Address issues with arena_rewind. Call it at the relevant points to ensure it's always at 0 on each recursion
// TODO: Make file handling (fts_open and fts_close calls) more efficient if needed
// TODO: Find out if there's a better way to skip Apple files
void test_directories_sync(Arena *arena_scratch, char *source_directory, char *target_directory) {
    if (source_directory == NULL || target_directory == NULL) {
        if (source_directory == NULL) {
            printf("End of source directory found for current recursion\n");
        }
        if (target_directory == NULL) {
            printf("End of target directory found for current recursion\n");
        }
        return;
    }
    size_t checkpoint = arena_checkpoint(arena_scratch);

    char *source_directory_fts[] = {source_directory, NULL};
    char *target_directory_fts[] = {target_directory, NULL};

    // Not entirely sure if `fts_open` needs error handling here
    FTS *source_handle = fts_open(source_directory_fts, FTS_PHYSICAL, by_name);
    if (source_handle == NULL) return;
    FTS *target_handle = fts_open(target_directory_fts, FTS_PHYSICAL, by_name);
    if (target_handle == NULL) return;

    FTSENT *source_children = test_children_get_list(source_handle);
    FTSENT *target_children = test_children_get_list(target_handle);

    FTSENT *s = source_children;
    FTSENT *t = target_children;
    while (s != NULL || t != NULL) {
        s = skip_apple_files(s);
        t = skip_apple_files(t);

        ////////////////////////////////////
        ///// IF TARGET/SOURCE IS NULL /////
        ////////////////////////////////////
        // If `t` is NULL, that means all the remaining source files are not in the target directory
        // So, we copy the remaining files in the current source directory to the target directory, and advance only `s`
        if (t == NULL) {
            while (s != NULL) {
                // Skip apple files
                s = skip_apple_files(s);
                char *source_path_copy = file_path_concatenate(arena_scratch, s->fts_path, s->fts_name);
                char *target_path_copy = file_path_concatenate(arena_scratch, target_directory, s->fts_name);
                if (is_a_directory(s)) {
                    printf("Source copy path:    %s\n", source_path_copy);
                    printf("Target copy path:    %s\n\n", target_path_copy);
                    //test_directory_copy(source_directory, target_directory);

                    // We only want to recurse into directories, not files
                    test_directories_sync(arena_scratch, source_path_copy, target_path_copy);
                }
                else if (is_a_file(s)) {
                    printf("Copying source file: %s\n", source_path_copy);
                    printf("Into target file:    %s\n\n", target_path_copy);
                    //file_copy(source_path_copy, target_path_copy);
                }
                s = s->fts_link;
            }
            fts_close(source_handle);
            fts_close(target_handle);
        }
        // If `s` is NULL, that means all the remaining target files are not in the source directory
        // So, we delete the remaining files in the current target directory, and advance only `t`
        // Note that we don't want to skip apple files here
        if (s == NULL) {
            while (t != NULL) {
                t = skip_apple_files(t);
                char *source_delete_path = file_path_concatenate(arena_scratch, source_directory, t->fts_name);
                char *target_delete_path = file_path_concatenate(arena_scratch, t->fts_path, t->fts_name);
                printf("Source delete path:        %s\n", source_delete_path);
                printf("Target delete path:        %s\n", target_delete_path);
                if (is_a_directory(t)) {
                printf("Deleting target directory: %s\n\n", target_delete_path);
                    //directory_delete(target_delete_path);

                    // We only want to recurse into directories, not files
                    test_directories_sync(arena_scratch, source_delete_path, target_delete_path);
                }
                else if (is_a_file(t)) {
                    printf("Deleting target file:      %s\n\n", target_delete_path);
                    //file_delete(target_delete_path);
                }
                t = t->fts_link;
            }
            fts_close(source_handle);
            fts_close(target_handle);
        }
        // Otherwise, `s` and `t` are not null and we can use string comparisons
        else {
            int string_comparison = strcmp(s->fts_name, t->fts_name);
            // If both files have the same name...
            if (string_comparison == 0) {
                // And both files are directories and their links are not null..
                if (is_a_directory(s) && is_a_directory(t) &&
                    (s->fts_link != NULL) && (t->fts_link != NULL)) {
                    // Skip to the next entry by moving both pointers forward
                    printf("Source:                %s and ...\n", s->fts_name);
                    printf("target:                %s\n", t->fts_name);
                    printf("Are both directories and have the same name\n");
                    char *source_path_skip = file_path_concatenate(arena_scratch, s->fts_path, s->fts_name);
                    char *target_path_skip = file_path_concatenate(arena_scratch, t->fts_path, t->fts_name);
                    printf("Source path for next recursion: `%s`\n", source_path_skip);
                    printf("Target path for next recursion: `%s`\n\n", target_path_skip);
                    s = s->fts_link;
                    t = t->fts_link;
                    test_directories_sync(arena_scratch, source_path_skip, target_path_skip);
                    // Figure out how to close directories
                }
            }
        }
    }


    // Cleanup
    printf("Arena before rewind: ");
    arena_print_info(arena_scratch);
    // NOTE: Arena contents pile up here, but that's fine for now. Can optimize later.
    arena_rewind(arena_scratch, checkpoint);
    printf("Arena after rewind: ");
    arena_print_info(arena_scratch);
    printf("\n\n");
}
