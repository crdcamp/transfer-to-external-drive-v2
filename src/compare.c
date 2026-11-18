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
    printf("Files:\n");
    while (ptr != NULL) {
        printf("%s\n", ptr->fts_name);
        ptr = ptr->fts_link;
    }
}

static int by_name(const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

 /* Visit each depth of the source and target file paths
     to ensure the target directory mirrors the source directory */
void compare_directories(char **source, char **target) {
    // Open the source and target directories while sorting them by name
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
    children_print_list(source_children_list);
    printf("\n");

    children_print_list(target_children_list);
    printf("\n");


    // MAKE A THOROUGH PLAN BEFORE CONTINUING HERE
    // Compare both sorted lists
    // Don't forget to also compare the modification dates
    // You might also want to check if the file size is greater or lesser
    // in addition to the modification date being more recent, but this might be unnecessary

    // While iterating through each entry in the lists:
        // If the list entries are the same, check the modification dates. If the mod dates are different, update the files accordingly
        // Otherwise move to the next depth
        // If the list entries are different, overwrite the source paths into the target and delete files not found in the source
        // DO NOT FORGET to do this on an entry-by-entry basis

    // Now, how do we set this recursion up so it continues down
    // the rest of the file paths?
    // I think this would involve passing the file paths that are directories
    // with an additional slash in them to access the next level.
    // This could also then be used recursively. You also already have
    // and arena function to accomplish this
    // Hopefully this recursion actually reaches all tree paths when implemented...
}
