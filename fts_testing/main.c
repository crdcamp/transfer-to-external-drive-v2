#include <stdio.h>

// "Directory entries"
#include <dirent.h>

// `fts` functionality
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include <copyfile.h>

void fts_testing(char **file);
void copyfile_testing(char *file);

int main(void) {
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
    struct _ftsent;

    // Two structures are defined (and typedef'd) in the include file <fts.h>.
    // The first is FTS, the structure that represents the file hierarchy itself.
    // The second is FTSENT, the structure that represents a file in the file hierarchy.
    // Normally, an FTSENT structure is returned for every file in the file hierarchy.
}
