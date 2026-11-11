#include "../include/arena_tree.h"

// TODO: Add file prints to tree_print
void tree_print(Node *n, int depth) {
    if (n == NULL) return;

    printf("%*s%s/\n", depth * 2, "", n->name);

    for (Node *c = n->first_child; c != NULL; c = c->next_sibling) {
        tree_print(c, depth + 1);
    }
}

// Allocate a new node for the tree and assign it to an arena
Node *new_node(Arena *arena, char *name) {
    Node *new_node = arena_allocate(arena, sizeof(Node), DEFAULT_ALIGNMENT);
    if (new_node == NULL) {
        return NULL;
    }

    new_node->name = name;
    new_node->parent = NULL;
    new_node->first_child = NULL;
    new_node->next_sibling = NULL;

    return new_node;
}

size_t arena_checkpoint(Arena *arena) {
    return arena->current_offset;
}

void arena_rewind(Arena *arena, size_t checkpoint) {
    arena->current_offset = checkpoint;
}

void tree_fill(Arena *arena, Arena *arena_temp, Node *node, char *file_path) {
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

            Node *child = new_node(arena, (char *)child_name_ptr);
            if (child == NULL) {
                fprintf(stderr, "Arena full: couldn't allocate node for '%s'\n", entry->d_name);
                closedir(dirp);
                exit(EXIT_FAILURE);
            }

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

            // Set an arena checkpoint to enable resetting the arena between recursive calls
            // (Saves a considerable amount of memory)
            size_t checkpoint = arena_checkpoint(arena_temp);

            // Concatenate the full file path with the child directory's name to pass to the next recursion
            char *subdir_array[3] = {file_path, "/", entry->d_name};
            size_t subdir_array_element_count = sizeof(subdir_array) / sizeof(subdir_array[0]);
            char *subdir_path = arena_string_concat(arena_temp, subdir_array, subdir_array_element_count);
            if (subdir_path == NULL) {
                fprintf(stderr, "Temp arena full at: %s\n", file_path);
                closedir(dirp);
                exit(EXIT_FAILURE);
            }
            //printf("%s\n", subdir_path);
            tree_fill(arena, arena_temp, child, subdir_path);

            // Rewind the arena to reset it between recursive calls
            arena_rewind(arena_temp, checkpoint);
        }
    }
    closedir(dirp);
}
