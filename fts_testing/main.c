#include <stdio.h>

// "Directory entries"
#include <dirent.h>

// `fts` (file tree stream) functionality
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include <stdlib.h>

#include <copyfile.h>

void fts_testing(char **file);
void fts_print_info(FTSENT *fts_entry);
void copyfile_testing(char *file);


int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *paths[] = {"TestDirs/TestSource", NULL};
    fts_testing(paths);
    return 0;
}

// typedef struct _ftsent {
//     unsigned short fts_info;     /* flags for FTSENT structure */
//     char          *fts_accpath;  /* access path */
//     char          *fts_path;     /* root path */
//     short          fts_pathlen;  /* strlen(fts_path) */
//     char          *fts_name;     /* filename */
//     short          fts_namelen;  /* strlen(fts_name) */
//     short          fts_level;    /* depth (-1 to N) */
//     int            fts_errno;    /* file errno */
//     long           fts_number;   /* local numeric value */
//     void          *fts_pointer;  /* local address value */
//     struct ftsent *fts_parent;   /* parent directory */
//     struct ftsent *fts_link;     /* next file structure */
//     struct ftsent *fts_cycle;    /* cycle structure */
//     struct stat   *fts_statp;    /* stat(2) information */
// } FTSENT;

void fts_print_info(FTSENT *fts_entry) {
    struct _ftsent;
    printf("access path:         %s\n", fts_entry->fts_accpath);
    printf("path:                %s\n", fts_entry->fts_path);
    if (fts_entry->fts_parent != NULL) {
        printf("parent name:         %s\n", fts_entry->fts_parent->fts_name);
    }
    printf("name:                %s\n", fts_entry->fts_name);
    printf("path length:         %i\n", fts_entry->fts_pathlen);
}

// DON'T FORGET WE ALSO HAVE `fts_children` to test out!
void fts_testing(char **paths) {
    // Returns a "handle" on the file hierarchy, which is then supplied
    // to the other `fts` functions
    // I believe the final argument `compar` can be used to compare files, or
    // maybe even hierarchies!

    // Two structures are defined (and typedef'd) in the include file <fts.h>.
    // The first is FTS, the structure that represents the file hierarchy itself.
    // The second is FTSENT, the structure that represents a file in the file hierarchy.
    // Normally, an FTSENT structure is returned for every file in the file hierarchy.
    FTS *fts_handle = fts_open(paths, FTS_PHYSICAL, NULL);
    if (fts_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    struct _ftsent;
    FTSENT *fts_entry = fts_read(fts_handle);
    if (fts_entry == NULL) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }

    fts_print_info(fts_entry);

    // So, what we need to do is somehow recurse through these entries (or the handle I'm
    // not 100% sure on that yet) while saving them to our own self-made struct

    // We will then compare two different trees (source and target) at a time, BUT,
    // instead of determining if they're the same and return a value, we instead fo this
    // while recursing through the trees

    // Don't forget, in addition to the more obvious information you'll need, that
    // you'll need to get the timestamp for the last edits in `fts_statp`
}
