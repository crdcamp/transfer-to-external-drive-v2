#include "structures.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *source[] = {"TestDirs/Source", NULL};
    char *target[] = {"TestDirs/Target", NULL};
    //fts_show_tree_info(source);

    fts_compare_dirs(source, target);

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
        printf("%s\n", ptr->fts_path);
        ptr = ptr->fts_link;
    }
}

FTSENT *children_sort_list(FTSENT *source_children_list, FTSENT *target_children_list) {
    printf("Working on it\n");
    return NULL;
}

void fts_compare_dirs(char **source, char **target) {
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

    // Create a linked list of all files in the current directory
    FTSENT *source_children_list = children_get_list(source_handle);
    FTSENT *target_children_list = children_get_list(target_handle);

    // Test print / list walk demonstration
    FTSENT *ptr = source_children_list;
    children_print_list(source_children_list);
    printf("\n");
    children_print_list(target_children_list);

    // Sort both directories alphabetically before continuing (merge sort)

    // Compare both sorted lists
    // Don't forget to also compare the modification dates
    // You might also want to check if the file size is greater or lesser
    // and the modification date is more recent, but this might be unnecessary

    // While iterating through each entry in the lists:
        // If the list entries are the same, move to the next depth
        // If the list entries are different, overwrite the source paths into the target

    // Now, how do we set this recursion up so it continues down
    // the rest of the file paths?
    // I think this would involve passing the file paths that are directories
    // with an additional slash in them to access the next level.
    // This could also then be used recursively. You also already have
    // and arena function to accomplish this
    // Hopefully this recursion actually reaches all tree paths when implemented...
}

















// void fts_show_tree_info(char **paths) {
//     // Returns a "handle" on the file hierarchy, which is then supplied
//     // to the other `fts` functions

//     // The function `fts_read()` returns a pointer to a structure describing one of the files in the file hierarchy.

//     // The function fts_children() returns a pointer to a linked list of structures,
//     // each of which describes one of the files contained in a directory in the hierarchy
//     // NOTE: You probably shouldn't use this for the purposes of this project

//     // Two structures are defined (and typedef'd) in the include file <fts.h>.
//     // The first is FTS, the structure that represents the file hierarchy itself.
//     // The second is FTSENT, the structure that represents a file in the file hierarchy.
//     // Normally, an FTSENT structure is returned for every file in the file hierarchy.

//     // Open the base path
//     FTS *fts_handle = fts_open(paths, FTS_PHYSICAL, NULL);
//     if (fts_handle == NULL) {
//         perror("fts_open");
//         exit(EXIT_FAILURE);
//     }

//     // Traverse all files and directories found in the base path
//     FTSENT *node;
//     while ((node = fts_read(fts_handle)) != NULL) {
//         // Print information on directories
//         if (node->fts_info == FTS_D) {
//             fts_print_entry_info(node);
//             printf("\n");
//         }
//         // Print information on files
//         if (node->fts_info == FTS_F) {
//             fts_print_entry_info(node);
//             printf("\n");
//         }
//     }
//     fts_close(fts_handle);
// }
