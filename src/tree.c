#include "../include/tree_arena.h"
#include <string.h>

void tree_print(Node *n, int depth) {
    if (n == NULL) return;

    printf("%*s%s/\n", depth * 2, "", n->name);

    for (const Node *c = n->first_child; c != NULL; c = c->next_sibling) {
        tree_print(c, depth + 1);
    }
}

// TODO: Convert this to an arena allocation (gonna be a huge pain in the butt)
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
void tree_fill(Node *node, char *file_path) {
    // If the node's parent is null, that means we're at the root
    // Therefore, we assign the full file path to it
    // TODO: Switch the logic here so that you're not checking twice
    // every iteration after the root node
    if (node->parent == NULL) node = new_node(file_path);

    // Otherwise, we assign the parent node's child's name to the new node
    else node = new_node(node->parent->first_child->name);

    // TODO: Make this file handling its own function (maybe)
    // All we need right now are the directories. No need to worry
    // about file names until a bit later. The actual copy function should
    // handle that
    DIR *dirp = opendir(file_path); // Directory stream
    if (dirp == NULL) {
        printf("Failed opening directory: %s\n", file_path);
        perror("`opendir` failed");
        // Return if you fail to open a directory
        return;
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
            // TO DO: FIND ALTERNATIVE TO STRDUP WHEN CONVERTING TO ARENA
            Node *child = new_node(strdup(entry->d_name));

            // Assign the parent to child
            child->parent = node;

            // Assign the child to parent
            node->first_child = child;

            // TODO: GET RID OF ASPRINTF AT SOME POINT. IT USES MALLOC AND WILL CAUSE COMPLICATIONS
            // We need to replace this with some kind of linkage to assemble
            // a full file path to pass to the recursion
            char *subdir_path;
            if (asprintf(&subdir_path, "%s/%s", file_path, entry->d_name) == -1) {
                printf("asprint error for subdir path concate: '%s' and '%s'\n", file_path, entry->d_name);
                perror("asprintf failed");
                continue;
            }
            // Now... how do we handle the next sibling?
            // Likely an if statement..... but how????

            tree_fill(child, subdir_path);
        }
    }
    closedir(dirp);
}
