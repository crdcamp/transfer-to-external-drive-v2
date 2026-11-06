#include <dirent.h>
#include <string.h>

#include "../include/tree.h"
#include "../include/transfer.h"

// We first need a way to grab the file names of each file
// and their relative place within the tree using a node struct

// SOME EXAMPLE CODE:
// if (is_dir) {
//     // Mark directories with a trailing slash
//     printf("/\n");
//     // Build subdirectory path and recurse
//     char *subdir_path;
//     if (asprintf(&subdir_path, "%s/%s", file_path, entry->d_name) == -1) {
//         perror("asprintf failed");
//         continue;
//     }
//     // Now we recurse
//     directory_traverse(subdir_path, indent + 1);
//     free(subdir_path);
// }

// Let's begin by just raw dawging a tree and making it by hand
Node *tree_create_test(char *root_path) {
    // Define the root
    Node *root;

    // Define the root node's name
    root->name = root_path;

    // Since it's the root, it's parent is NULL
    root->parent = NULL;

    // TODO: Make this file handling its own function (maybe)
    // To get the first child, we need to use the functions found in
    // `transfer.c`
    // All we need right now are the directories. No need to worry
    // about file names until a bit later
    printf("Opening directory: `%s`\n", root_path);
    DIR *dirp = opendir(root_path); // Directory stream
    if (dirp == NULL) {
        perror("`opendir` failed");
        // Return NULL if you fail to open a directory
        return NULL;
    }

    struct dirent *entry; // "Directory entry"
    while ((entry = readdir(dirp)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Skip symlinks to avoid infinite loops
        if (entry->d_type == DT_LNK) {
            continue;
        }

        // Check if entry is a directory
        int is_dir = 0;
        if (entry->d_type == DT_DIR) {
            is_dir = 1;
        }
        // I'm not gonna bother with all the fallbacks included
        // in the follow-along code here
        else {
            is_dir = 0;
        }

        if (is_dir) {
            // Assign the directory to child
            Node *child;
            strcpy(child->name, entry->d_name);
            child->parent = root;
            return child;
        }
    }

    // The root is always a directory
    root->is_dir = 1; // 1 for true, 0 for false
}
