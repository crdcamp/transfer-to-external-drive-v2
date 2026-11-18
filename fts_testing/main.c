#include "structures.h"

int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *paths[] = {"TestDirs", NULL};
    fts_show_tree_info(paths);

    return 0;
}

void fts_print_entry_info(FTSENT *node) {
    struct _ftsent;
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
void fts_show_tree_info(char **paths) {
    // Returns a "handle" on the file hierarchy, which is then supplied
    // to the other `fts` functions

    // The function `fts_read()` returns a pointer to a structure describing one of the files in the file hierarchy.

    // The function fts_children() returns a pointer to a linked list of structures,
    // each of which describes one of the files contained in a directory in the hierarchy
    // NOTE: You probably shouldn't use this for the purposes of this project

    // Two structures are defined (and typedef'd) in the include file <fts.h>.
    // The first is FTS, the structure that represents the file hierarchy itself.
    // The second is FTSENT, the structure that represents a file in the file hierarchy.
    // Normally, an FTSENT structure is returned for every file in the file hierarchy.

    // Open the base path
    FTS *fts_handle = fts_open(paths, FTS_PHYSICAL, NULL);
    if (fts_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Traverse all files and directories found in the base path
    FTSENT *node;
    while ((node = fts_read(fts_handle)) != NULL) {
        // Print information on directories
        if (node->fts_info == FTS_D) {
            fts_print_entry_info(node);
            printf("\n");
        }
        // Print information on files
        if (node->fts_info == FTS_F) {
            fts_print_entry_info(node);
            printf("\n");
        }
    }
    fts_close(fts_handle);

    // Copy the relevant fields of the struct into the arena. They can then be used to
    // do the rest of the file handling

    // Now... before you go nuts with the coding, you'll need to read through all of the
    // `fts` docs, as there might be some tools in there you'll need. Also make sure
    // to fully read over both the structs just so you're not forgetting any useful info
    // that you may need for the structs as well

    // NOTE: There's an `fts_pointer` field, which can easily be used for getting information
    // more quickly. This might come in handy

    // NOTE: You'll need a way to create directories. You can compare both file structures until
    // they aren't the same anymore. Once you reach one that doesn't match, you overwrite newer files with
    // older files, delete files not found in the source directory, and make directories that exist in the
    // source directory, but not the target directory
}
