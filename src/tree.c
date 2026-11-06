#include "../include/tree_arena.h"

void tree_print(Node *n, int depth) {
    if (n == NULL) return;

    printf("%*s%s/\n", depth * 2, "", n->name);

    for (const Node *c = n->first_child; c != NULL; c = c->next_sibling) {
        tree_print(c, depth + 1);
    }
}

// TODO: Convert this to an arena allocation
Node *new_node(char *name) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->name = name;
    new_node->parent = NULL;
    new_node->first_child = NULL;
    new_node->next_sibling = NULL;

    return new_node;
}

// Start by just making the tree... THEN try to add it to an arena
Node *tree_create(char *root_path) {
    // Define the root
    Node *root = new_node(root_path);
    // Define the root node's name
    root->name = root_path;
    printf("Parent name: %s\n", root->name);
    // Since it's the root, it's parent is NULL
    root->parent = NULL;

    // TODO: Make this file handling its own function (maybe)
    // All we need right now are the directories. No need to worry
    // about file names until a bit later. The actual copy function should
    // handle that
    DIR *dirp = opendir(root_path); // Directory stream
    if (dirp == NULL) {
        printf("Failed opening directory: %s\n", root_path);
        perror("`opendir` failed");
        // Return NULL if you fail to open a directory
        return NULL;
    }

    // Iterate through the directory while looking for subdirectories
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
        // in the follow-along code here. Will implement later
        else {
            is_dir = 0;
        }

        if (is_dir) {
            // Create a new node for child and assign
            Node *child = new_node(entry->d_name);
            printf("Child name: %s\n\n", child->name);
            // Assign the parent to child
            child->parent = root;
            // Assign the child to parent
            root->first_child = child;
            char *subdir_path;
            if (asprintf(&subdir_path, "%s/%s", root_path, entry->d_name) == -1) {
                printf("asprint error for subdir path concate: '%s' and '%s'\n", root_path, entry->d_name);
                perror("asprintf failed");
                continue;
            }
            // Now... how do we handle the next sibling?
            // Likely an if statement..... but how????
            tree_create(subdir_path);
        }
    }

    return root;
}
