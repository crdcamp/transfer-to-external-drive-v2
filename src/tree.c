#include "../include/arena_tree.h"

void tree_print(Node *n, int depth) {
    if (n == NULL) return;

    printf("%*s%s/\n", depth * 2, "", n->name);

    for (Node *c = n->first_child; c != NULL; c = c->next_sibling) {
        tree_print(c, depth + 1);
    }
}

// TODO: Convert this to an arena allocation (gonna be a huge pain in the butt)
// Update: Nevermind. Should be pretty easy actually...
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

// TO DO: Implement file structure into the tree as well
// TO DO: Add arena expansion when it runs out of space
void tree_fill(Arena *arena, Node *node, char *file_path) {
    // Define a directory pointer to access metadata on directories
    DIR *dirp = opendir(file_path);
    if (dirp == NULL) {
        printf("Failed opening directory: %s\n", file_path);
        perror("`opendir` failed");
        return;
    }

    // Recurse through each directory/subdirectory
    struct dirent *entry; // `dirent` = "directory entry"
    Node *previous_child = NULL;
    while ((entry = readdir(dirp)) != NULL) {
        // Skip "." and ".." (used for dictating the current or previous directory respectively)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Skip symlinks to avoid infinite loops
        if (entry->d_type == DT_LNK) {
            continue;
        }
        // Check if entry is a directory
        bool is_dir = false;
        if (entry->d_type == DT_DIR) {
            is_dir = true;
        }
        // There are additional fallbacks that could be added here, but I feel they're unnecessary
        else {
            is_dir = false;
        }

        if (is_dir) {
            // Create a new node for child and assign
            size_t child_name_ptr_size = strlen(entry->d_name)+1;
            void *child_name_ptr = arena_string_duplicate(arena, entry->d_name, child_name_ptr_size);
            if (child_name_ptr == NULL) {
                fprintf(stderr, "Arena full: couldn't store name '%s'\n", entry->d_name);
                closedir(dirp);
                exit(EXIT_FAILURE);
            }
            Node *child = new_node((char *)child_name_ptr);

            // Child's parent will always be the node from the previous recursion
            child->parent = node;

            // If the parent DOES NOT have a child from previous recursions,
            // then this is their first child
            if (previous_child == NULL) {
                node->first_child = child;
            // If the parent DOES have a child from previous recursions,
            // then the new child is the next sibling
            } else {
                previous_child->next_sibling = child;
            }
            // The current child is now be the previous child for the next recursive case
            previous_child = child;

            // TODO: GET RID OF ASPRINTF. IT USES MALLOC AND WILL CAUSE COMPLICATIONS
            // So, how do begin?
            // Do we use `arena_string_duplicate`?, or do we make a new function for this?
            // First, we definitely need the size of the path... yet that can't be found
            // until the strings are assembled together
            // So, let's think of a function prototype
            // We need the arena, the destination to assign the concatenated string,
            // and two arguments for both strings to concatenate

            // POST ATTEMPT THOUGHTS:
            // There's gotta be a better way to do this. I need a function that can pass
            // any number of strings and concatenate them with any desired delimiter
            // Not only would that be useful later on, it would also be useful literally
            // any time I need to concatenate strings with a delimiter
            // But.... how the hell do I do it?
            // Unfortunately (because I don't feel like learning this at the moment),
            // Your only option is a dynamic array arena allocator. Tsoding's code
            // would be a good start
            // Actually... you can likely implement a general dynamic arena allocation
            // using the `?` thingy. This should probably be a set amount of bytes
            // as a first implementation to keep things simple
            char *subdir_path = NULL;
            if (asprintf(&subdir_path, "%s/%s", file_path, entry->d_name) == -1) {
                printf("asprint error for subdir path concate: '%s' and '%s'\n", file_path, entry->d_name);
                perror("asprintf failed");
                continue;
            }
            tree_fill(arena, child, subdir_path);
        }
    }
    closedir(dirp);
}
