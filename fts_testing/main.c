#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

void recurse_through_file(char **file_path) {
    FTS *fts_handle = fts_open(file_path, FTS_PHYSICAL, NULL);
    FTSENT *entry;
    while ((entry = fts_read(fts_handle)) != NULL) {

    }
}

int main(void) {
    char *file = "../TestDirs/Target";
    recurse_through_file(file);
}
