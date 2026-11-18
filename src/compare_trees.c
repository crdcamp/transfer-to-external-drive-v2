#include "../include/compare.h"
#include "../include/arena.h"

int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *source[] = {"TestDirs/Source", NULL};
    char *target[] = {"TestDirs/Target", NULL};
    //fts_show_tree_info(source);

    compare_directories(source, target);

    return 0;
}

// We can abandon the entire arena approach,
// but we can use a scratch arena here as well for string and path length handling
// In general though, we should primarily be relying on `fts`
// First, we need a function that only takes in a source and target parameter and a scratch arena
// We need to compare the source and the destination as we're iterating over the two
// Not sure how we'd do that, but we have a good enough idea of the data structure to at least get started

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
    while (ptr != NULL) {
        printf("%s\n", ptr->fts_name);
        ptr = ptr->fts_link;
    }
}

void *children_sort_list(FTSENT *children_list) {
    // Get the file names of each file in the linked list
    char *names[] = {0};
    int i = 0;

    // Need to allocate to the arena for this
    // Let's put a pin in this and move these functions into
    // the main project since this seems like it's actually
    // going to work out
    while (children_list != NULL) {
        names[i] = children_list->fts_name;
        i+=1;
    }

    for (int j = 0; j < i; j++) {
        printf("Name: %s", names[i]);
    }
}

 /* Visit each depth of the source and target file paths
     to ensure the target directory mirrors the source directory */
void compare_directories(char **source, char **target) {
    // Open the source and target directories
    FTS *source_handle = fts_open(source, FTS_PHYSICAL, NULL);
    if (source_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }
    FTS *target_handle = fts_open(target, FTS_PHYSICAL, NULL);
    if (target_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Create a linked list of all files/directories in the current directory
    FTSENT *children_list = children_get_list(source_handle);
    FTSENT *target_children_list = children_get_list(target_handle);

    // Test print
    FTSENT *ptr = children_list;
    children_print_list(children_list);
    printf("\n");
    children_print_list(target_children_list);
    printf("\n");

    // Sort both directories alphabetically before continuing (merge sort)


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
