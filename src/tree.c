#include "../include/arena_tree.h"
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
void tree_fill(Arena *arena, Node *node, char *file_path) {
    // All we need from this function is a tree that directly mirrors the
    // tree structure of the directories. Since the files within the directories
    // will be found by passing the tree to another function, we don't need to
    // assemble the file paths yet.

    // In short, we're creating a sort of "metadata" of the directory structure
    // that's mapped to a tree that we'll need to traverse again when copying
    // (Actually, in hindsight, this should also probably be part of this function)
    // A problem for later though, as regardless we'd likely need to call a helper
    // function to achieve this

    // POSSIBLE FUTURE ADJUSTMENT: You could calculate the size of the tree as you
    // create it and return a struct containing the tree and its size in bytes

    // OR, EVEN BETTER: This could also involve creating and allocating to an arena
    // at the end of the function (assuming you calculate the size of the tree with each
    // recursive step), rather than initializing an arena beforehand.
    // This would also remove the need for the struct recommended above
    // However, this is likely a final requirement, as this results in a need
    // for an arena resizing function: the part of the arena you have very little
    // understanding of

    DIR *dirp = opendir(file_path); // Directory stream
    if (dirp == NULL) {
        printf("Failed opening directory: %s\n", file_path);
        perror("`opendir` failed");
        // Return if you fail to open a directory
        return;
    }

    // Recurse each directory while looking for subdirectories
    struct dirent *entry; // "Directory entry"
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
        // I'm not gonna bother with all the fallbacks included
        // in the follow-along code here. Will implement later
        // ... or... maybe not. They seem a bit excessive
        else {
            is_dir = false;
        }

        if (is_dir) {
            // Create a new node for child and assign
            // TO DO: FIND ALTERNATIVE TO STRDUP WHEN CONVERTING TO ARENA. REQUIRES MALLOC AND FREE,
            // DEFEATS THE PURPOSE OF THE ARENA
            Arena_String child_arena_string = arena_string_duplicate(arena, entry->d_name);
            Node *child = new_node(child_arena_string.string);
            printf("CHILD NAME: %s\n", child->name);
            // FREE THE ARENA STRING DUPLICATE!!!!!!!

            // Assign the child's parent
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
            // The current child will now be the previous child for the next
            // recursive case
            previous_child = child;

            // TODO: GET RID OF ASPRINTF AT SOME POINT. IT USES MALLOC AND WILL CAUSE COMPLICATIONS

            // It's likely we need to implement the arena before replacing `asprintf`. The same goes for `strdup`
            // We can avoid `malloc` and `free` this way. However, that begs the question of whether or not
            // we should store and free these string values in a separate arena.

            // Regardless, it seems like you'll need to create your own versions of `asprintf` and `strdup`
            // in order to make this happen. Given that these values will be freed immediately after
            // their usage, we could just keep this all to a single arena. Let's just see what happens for now

            // Update: I think it'd be best to split the tree and smaller allocations into two separate arenas
            // The smaller one can be used for functions like `asprintf` and `strdup`.
            // were gonna begin with some basic testing in a separate file called `testing.c`

            char *subdir_path;
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
