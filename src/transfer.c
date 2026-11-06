#include "../include/transfer.h"

int main(void) {
    // Define an array of 3 source directories
    const char *source_dirs[] = {"TestDirs/Code", "TestDirs/Documents"};
    size_t source_dirs_len = sizeof(source_dirs) / sizeof(source_dirs[0]);

    // Define a target directory (leaving as an array for later implementations)
    const char *target_dirs[] = {"TestDirs/TargetDir"};
    size_t target_dir_len = sizeof(target_dirs) / sizeof(target_dirs[0]);

    for (size_t i = 0; i < source_dirs_len; i++) {
        DIR *dirp = opendir(source_dirs[i]);
        if (dirp == NULL) {
            perror("`open_directory failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
