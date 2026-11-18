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

FTSENT *get_children_list(FTS *handle) {
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

void fts_compare_dirs(char **source, char **target) {
    // Open the source directory
    FTS *source_handle = fts_open(source, FTS_PHYSICAL, NULL);
    if (source_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    FTSENT *source_children = get_children_list(source_handle);
    FTSENT *ptr = source_children;
    while (ptr != NULL) {
        printf("Path: %s\n", source_children->fts_path);
        ptr = ptr->fts_link;
    }

    // Get a linked list of the files in the target directory

    // Get a linked

    // Get an array of the directories in the source and sort


    // Get an array of the files in the source and sort

    // Get an array of the files in the target and sort

    // Get an array of directories in the target and sort

    // Compare the source and target directory arrays

    // Compare the source and target file arrays

    // Don't forget to implement the modification date check as well
}

void fts_print_entry_info(FTSENT *node) {
    printf("Full path:         %s\n", node->fts_path);
    printf("Name:              %s\n", node->fts_name);
    if (strcmp(node->fts_parent->fts_name, "") == 0)
        printf("Parent name:       %s\n", "No parent");
    else
        printf("Parent name:       %s\n", node->fts_parent->fts_name);
    printf("Path length:       %i\n", node->fts_pathlen);
    printf("Depth:             %i\n", node->fts_level);

    // The pointer field is provided for the use of the application program and
    // is not modified by the fts functions. It is initialized to NULL. Keep
    // for now since it might be useful later
    printf("Pointer:           %p\n", node->fts_pointer);
    printf("Last modification: %s", ctime(&node->fts_statp->st_mtime));
}

// DON'T FORGET WE ALSO HAVE `fts_children` to test out!
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
