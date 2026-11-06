#include <stdio.h>

int main(void) {
    // Define an array of 3 source directories
    const char *source_dirs[] = {"../TestDirs/SourceDir1", "../TestDirs/SourceDir2", "../TestDirs/SourceDir3"};
    size_t source_dirs_size = sizeof(source_dirs) / sizeof(source_dirs[0]);
    printf("Source dirs size: %zu\n", source_dirs_size);

    // Define a target directory (leaving as an array for later implementations)
    const char *target_dirs[] = {"../TestDirs/TargetDir"};
    size_t target_dir_size = sizeof(target_dirs) / sizeof(target_dirs[0]);
    printf("Source dirs size: %zu\n", target_dir_size);


    return 0;
}
