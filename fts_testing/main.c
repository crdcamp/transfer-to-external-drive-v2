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
void copyfile_testing(char *file);

int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *paths[] = {"TestDirs/TestSource", NULL};
    fts_testing(paths);
    return 0;
}

void fts_testing(char **paths) {
    // Returns a "handle" on the file hierarchy, which is then supplied
    // to the other `fts` functions
    // I believe the final argument `compar` can be used to compare files, or
    // maybe even hierarchies!
    FTS *fts_handle = fts_open(paths, FTS_PHYSICAL, NULL);
    if (fts_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
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

    struct _ftsent;
    FTSENT *fts_entry = fts_read(fts_handle);
    printf("access path: %s\n", fts_entry->fts_accpath);
    printf("path:        %s\n", fts_entry->fts_path);
    printf("name:        %s\n", fts_entry->fts_name);

    // Two structures are defined (and typedef'd) in the include file <fts.h>.
    // The first is FTS, the structure that represents the file hierarchy itself.
    // The second is FTSENT, the structure that represents a file in the file hierarchy.
    // Normally, an FTSENT structure is returned for every file in the file hierarchy.
}
