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
    while (source_ptr != NULL) {
        char *source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
        char *target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);

        // Boolean logic:
        // Rest of boolean logic is only used once, so they don`t need functions
        bool source_file_is_a_directory = is_a_directory(source_ptr);
        bool target_file_is_a_directory = is_a_directory(target_ptr);
        bool source_file_is_a_file = is_a_file(source_ptr);
        bool target_file_is_a_file = is_a_file(target_ptr);

        bool file_names_are_same;
        if (strcmp(source_ptr->fts_name, target_ptr->fts_name) == 0) {
            file_names_are_same = true;
        } else file_names_are_same = false;

        bool source_file_is_newer;
        if (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime) {
            source_file_is_newer = true;
        } else source_file_is_newer = false;

        bool source_at_end_of_list;
        bool target_at_end_of_list;
        if (source_ptr->fts_link == NULL) {
            source_at_end_of_list = true;
        } else source_at_end_of_list = false;
        if (target_ptr->fts_link == NULL) {
            target_at_end_of_list = true;
        } else target_at_end_of_list = false;

        // Application of boolean logic:
        /////////////////////////////////////////////
        ///// IF SOURCE AND TARGET NAMES MATCH //////
        /////////////////////////////////////////////
        // If the source and target are directories, and their names match, then continue to the next file pair,
        // since we don`t know if the files in their subdirectories have been changed, we can leave further editing of the subdirectories to the next recursion
        if (source_file_is_a_directory &&
            target_file_is_a_directory &&
            file_names_are_same) {

            printf("Source `%s` and...\ntarget `%s`\n...are directories and have matching names. Moving to next list item\n\n",
                source_full_path, target_full_path);

            source_ptr = source_ptr->fts_link;
            target_ptr = target_ptr->fts_link;
            continue;
        }
        // If the source and target files are files, and their names match, and the source was edited more recently
        // Overwrite the source file to the target file and continue to the next file pair
        if (source_file_is_a_file &&
            target_file_is_a_file &&
            file_names_are_same &&
            source_file_is_newer) {

            printf("Source `%s` and...\ntarget `%s`\n...are files, have matching names, and the source was edited more recently than the target. Overwriting source to target\n\n",
                source_full_path, target_full_path);

            //file_copy(source_full_path, target_full_path);
            source_ptr = source_ptr->fts_link;
            target_ptr = target_ptr->fts_link;
            continue;
        }

        ///////////////////////////////////////////////////
        ///// IF SOURCE AND TARGET NAMES DO NOT MATCH /////
        ///////////////////////////////////////////////////
        // If the source and target files are directories, and their names do not match,
        // Check if the next file in the target list matches the current source file
        if (source_file_is_a_directory &&
            target_file_is_a_directory &&
            !file_names_are_same) {

            printf("Source directory `%s` and...\ntarget directory `%s`\n...are directories and do not have matching names.\nChecking next source file for match...\n",
                source_full_path, target_full_path);

            // If the next file in the source list is not null, is a directory, and has the same name as the current item in the target list,
            // that means that the current source directory file is not in the target directory, so copy it over
            if ((source_ptr->fts_link != NULL) &&
                (is_a_directory(source_ptr->fts_link)) &&
                (strcmp(source_ptr->fts_link->fts_name, target_ptr->fts_name) == 0)) {
                source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, "");

                printf("Next source directory `%s` matches current target file `%s`.\nThis means that current source directory `%s` is not in the target directory. Copying current source file `%s to target `%s`\n",
                source_ptr->fts_link->fts_name, target_ptr->fts_name, source_ptr->fts_name, source_full_path, target_full_path);

                //directory_copy(source_full_path, target_full_path);
                printf("Moving source from directory `%s` to directory ", source_ptr->fts_name);
                source_ptr = source_ptr->fts_link;
                printf("`%s`\n", source_ptr->fts_name);
                printf("Current target directory: %s\n\n", target_ptr->fts_name);
                continue;
            }
            // If the next file in the source list is not null, is a directory, and does not have the same name as the current directory in the target list,
            // that means that that the next source file is not in the target list, so copy it over
            // THE CURRENT DIRECTORY STRUCTURE CALLS THIS CONDITION
            if (is_a_directory(source_ptr->fts_link) &&
                strcmp(source_ptr->fts_link->fts_name, target_ptr->fts_name) != 0) {
                source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_link->fts_path, source_ptr->fts_link->fts_name);
                target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, "");

                printf("Next source directory `%s` does not match the current target directory `%s`.\nThis means that the next source directory is not in the target directory. Copying next source file `%s` to target `%s\n\n",
                    source_ptr->fts_link->fts_name, target_ptr->fts_name, source_full_path, target_full_path);

                //file_copy(source_full_path, target_full_path);


                // Iterate source_ptr over one
                source_ptr = source_ptr->fts_link;
                printf("CURRENT SOURCE DIRECTORY: %s\n", source_ptr->fts_name);
                printf("CURRENT TARGET DIRECTORY: %s\n", target_ptr->fts_name);
                // Iterate target_ptr until it is == source_ptr
                // Implicitly, every file in between the current target directory and the new next source directory
                // is not in the source directory
                while ((strcmp(target_ptr->fts_name, source_ptr->fts_name) != 0) && (target_ptr->fts_link != NULL)) {
                    if (target_ptr->fts_name != source_ptr->fts_name) {
                        printf("Target directory `%s` != source directory `%s`. Deleting", target_ptr->fts_name, source_ptr->fts_name);
                        // Don't forget to also account for non-matching files as well
                    }
                    printf("MOVING TARGET `%s` TO `%s`\n", target_ptr->fts_name, target_ptr->fts_link->fts_name);
                    target_ptr = target_ptr->fts_link;
                }
                printf("CURRENT TARGET DIRECTORY: %s\n", target_ptr->fts_name);
                continue;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        ///// IF SOURCE/TARGET IS A FILE AND TARGET/SOURCE IS A DIRECTORY /////
        ///////////////////////////////////////////////////////////////////////
        // Walk through the logic in your notes on this before continuing
        // Also adjust the directories to reflect this scenario
        // As a starting point, I`m pretty sure this mostly just involves deleting
        // target files. Since the lists should always be aligned, any instance where
        // they`re not indicates that you should either copy or delete something

        /////////////////////////////////////////
        ///// IF EITHER LIST IS AT ITS END //////
        /////////////////////////////////////////
        // If the source and target are at the end of their lists,
        // Move on to the next iteration
        // I`m almost certain the while loop condition takes care of this

        // If the source is at the end of its list and the target is not,
        // Then delete the rest of the files in the target
        if (source_at_end_of_list && !target_at_end_of_list) {

            printf("End of source file directories reached at `%s`. Deleting remaining target files...\n", source_full_path);

            while (target_ptr != NULL) {
                target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
                if (target_ptr->fts_info == FTS_D) {

                    printf("Deleting extra target directory `%s`\n", target_full_path);

                    //directory_delete(target_full_path);
                }
                if (target_ptr->fts_info == FTS_F) {

                    printf("Deleting extra target file `%s`\n", target_full_path);

                    //file_delete(target_full_path);
                }
                target_ptr = target_ptr->fts_link;
            }
            printf("\n");
            break;
        }
        // If the target is at the end of its list and the source is not,
        // then copy over the rest of the source files
        if (target_at_end_of_list && !source_at_end_of_list) {

            printf("End of target file directories reached at `%s`. Copying remaining source files...\n", target_full_path);

            while (source_ptr != NULL) {
                source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
                target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, source_ptr->fts_name);
                if (source_ptr->fts_info == FTS_D) {

                    printf("Copying extra source directory `%s` to target `%s`\n", source_full_path, target_full_path);

                    //directory_copy(source_full_path, target_full_path);
                }
                if (source_ptr->fts_info == FTS_F) {

                    printf("Copying extra source file `%s` to target `%s`\n", source_full_path, target_full_path);

                    //file_copy(source_full_path, target_full_path);
                }
                source_ptr = source_ptr->fts_link;
            }
            printf("\n\n");
            continue;
        }
    }
    // NOTE: When you add recursion: create a function that returns a pointer to the next directory string
    // that will be passed here. Then call this function in the recursion call
    arena_rewind(arena_scratch, checkpoint);
    fts_close(source_handle);
    fts_close(target_handle);
}
