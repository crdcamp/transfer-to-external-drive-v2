#include "../include/transfer.h"
#include "../include/arena.h"
#include "../include/tree.h"

int main(void) {
    // Define an array of 3 source directories
    // const char *source_dirs[] = {"TestDirs/Code", "TestDirs/Documents"};
    // size_t source_dirs_len = sizeof(source_dirs) / sizeof(source_dirs[0]);

    // // Define a target directory (leaving as an array for later implementations)
    // const char *target_dirs[] = {"TestDirs/TargetDir"};
    // size_t target_dir_len = sizeof(target_dirs) / sizeof(target_dirs[0]);

    // for (size_t i = 0; i < source_dirs_len; i++) {
    //     dirs_list_all(source_dirs[i]);
    // }

    // Let's begin with a small example directory
    const char *small_ex_dir = "TestDirs/SmallDir";
    directory_traverse(small_ex_dir, 4);

    // Now let's create an arena to store this directory.
    // We'll give it a bunch of space for preliminary testing
    Arena ex_arena;
    byte ex_arena_buffer[1024];
    size_t ex_arena_buffer_length = sizeof(ex_arena_buffer) / sizeof(ex_arena_buffer[0]);

    arena_init(&ex_arena, ex_arena_buffer, ex_arena_buffer_length);
    arena_alloc_align(&ex_arena, sizeof(ex_arena_buffer), DEFAULT_ALIGNMENT);

    // Now we need to rewrite the `directory_traverse` function to actually return something
    // Probably gonna want to do this from the ground up
    // First thing we need to focus on for this new function, is that we want to somehow store
    // the directory structure into a tree

    // We'll begin by making a tree by hand
    tree_create_test(small_ex_dir);

    return 0;
}

int dirs_list_all(const char *file_path) {
    printf("Opening directory: '%s'\n", file_path);
    DIR *dirp = opendir(file_path);
    if (dirp == NULL) {
        perror("`open_directory failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Directory: '%s' successfully opened\n", file_path);
    }

    struct dirent *entry; // "Directory entry"
    printf("Files found in directory '%s':\n", file_path);
    while ((entry = readdir(dirp)) != NULL) {
        // Skip "." (current dir) and ".." (parent dir)
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' ||
            (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }
        printf("%s\n", entry->d_name);  // Print the filename
    }

    // Check if `readdir()` failed
    if (errno != 0) {
        perror("`readdir` failed");
        closedir(dirp);
        exit(EXIT_FAILURE);
    }

    if (closedir(dirp) == -1) {
        perror("`closedir` failed");
        exit(EXIT_FAILURE);
    }

    printf("\n\n\n");
    return EXIT_SUCCESS;
}

void directory_traverse(const char *file_path, int indent) {
    DIR *dirp = opendir(file_path);
    if (dirp == NULL) {
        perror("`opendir failed");
        // Continue traversal even if one directory fails
        // (Might want to change this behavior later on)
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Skip symlinks to avoid infinite loops
        if (entry->d_type == DT_LNK) {
            continue;
        }

        // Indent for readability (shows hierarchy)
        for (int i = 0; i < indent; i++) printf("  ");
        printf("%s", entry->d_name);

        // Check if entry is a directory
        int is_dir = 0;
        if (entry->d_type == DT_DIR) {
            is_dir = 1;
        }
        else if (entry->d_type == DT_UNKNOWN) {
            // Fallback: use lstat() to check if it's a directory (and not a symlink)
            char *full_path;
            if (asprintf(&full_path, "%s/%s", file_path, entry->d_name) == -1) {
                perror("asprintf failed");
                continue;
            }

            struct stat statbuf;
            if (lstat(full_path, &statbuf) == -1) {
                perror("lstat failed");
                free(full_path);
                continue;
            }

            if (S_ISDIR(statbuf.st_mode) && !S_ISLNK(statbuf.st_mode)) {
                is_dir = 1;
            }
            free(full_path);
        }

        if (is_dir) {
            // Mark directories with a trailing slash
            printf("/\n");
            // Build subdirectory path and recurse
            char *subdir_path;
            if (asprintf(&subdir_path, "%s/%s", file_path, entry->d_name) == -1) {
                perror("asprintf failed");
                continue;
            }
            // Now we recurse
            directory_traverse(subdir_path, indent + 1);
            free(subdir_path);
        }
        else {
            printf("\n");
        }
    }

    if (errno != 0) {
        perror("`readdir` failed");
    }

    closedir(dirp);
}
