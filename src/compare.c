#include "../include/compare.h"
#include "../include/arena.h"

FTSENT *children_get_list(FTS *handle) {
    FTSENT *list_start = fts_read(handle);
    if (list_start == NULL) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }
    FTSENT *children_list = fts_children(handle, 0);
    if (children_list == NULL) {
        perror("fts_children");
        exit(EXIT_FAILURE);
    }
    return children_list;
}

void children_print_list(FTSENT *children_list) {
    FTSENT *ptr = children_list;
    printf("Parent: %s\n", ptr->fts_parent->fts_name);
    printf("Depth: %i\n", ptr->fts_level);
    printf("Files: ");
    while (ptr != NULL) {
        printf("%s ", ptr->fts_name);
        ptr = ptr->fts_link;
    }
    printf("\n");
}

static int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

 /*
    Visit each depth level of the source and target directory paths
    to ensure the target directory mirrors the source directory.
    The target directory is edited in-place and deletes/updates
    files according to the source directory's file hierarchy structure.
    A scratch `Arena` must be provided for string operations.
*/
void compare_directories(Arena *arena_scratch, char **source, char **target) {
    // Open the source and target directories while sorting them `by_name`
    FTS *source_handle = fts_open(source, FTS_PHYSICAL, by_name);
    if (source_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }
    FTS *target_handle = fts_open(target, FTS_PHYSICAL, by_name);
    if (target_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Create a linked list of all files/directories in the current directory
    FTSENT *source_children_list = children_get_list(source_handle);
    FTSENT *target_children_list = children_get_list(target_handle);

    // Test print
    // children_print_list(source_children_list);
    // printf("\n");

    // children_print_list(target_children_list);
    // printf("\n");

    // Iterate through the `source_children_list` and the `target_children_list` at the same time
    int i = 1;
    FTSENT *source_ptr = source_children_list;
    FTSENT *target_ptr = target_children_list;
    while (source_ptr != NULL) {
        printf("Iteration %i\n", i);
        printf("Current source file: %s\n", source_ptr->fts_name);
        printf("Current target file: %s\n", target_ptr->fts_name);
        printf("\n");

        if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
            printf("Target directory is larger than source! Next target directory: %s\n", target_ptr->fts_link->fts_name);
            break;
        }
        else {
            source_ptr = source_ptr->fts_link;
            target_ptr = target_ptr->fts_link;
        }

        i+=1;
    }

    // Now, how do we set this recursion up so it continues down
    // the rest of the file paths?
    // I think this would involve passing the file paths that are directories
    // with an additional slash in them to access the next level.
    // This could also then be used recursively. You also already have
    // and arena function to accomplish this
    // Hopefully this recursion actually reaches all tree paths when implemented...
}
