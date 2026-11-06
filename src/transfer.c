#include "../include/transfer.h"

int main(void) {
    // Define an array of 3 source directories
    const char *source_dirs[] = {"TestDirs/Code", "TestDirs/Documents"};
    size_t source_dirs_len = sizeof(source_dirs) / sizeof(source_dirs[0]);

    // Define a target directory (leaving as an array for later implementations)
    const char *target_dirs[] = {"TestDirs/TargetDir"};
    size_t target_dir_len = sizeof(target_dirs) / sizeof(target_dirs[0]);

    for (size_t i = 0; i < source_dirs_len; i++) {
        dirs_list_all(source_dirs[i]);
    }

    return 0;
}

int dirs_list_all(const char *file_path) {
    printf("Opening directory: '%s'\n", file_path);
    DIR *dirp = opendir(file_path);
    if (dirp == NULL) {
        perror("`open_directory failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Directory: '%s' successfully opened\n", file_path);
    }

    struct dirent *entry; // "Directory entry"
    printf("Files found in directory '%s':\n", file_path);
    while ((entry = readdir(dirp)) != NULL) {
        // Skip "." (current dir) and ".." (parent dir)
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' ||
            (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }
        printf("%s\n", entry->d_name);  // Print the filename
    }
    printf("\n\n\n");

    // Check if `readdir()` failed
    if (errno != 0) {
        perror("`readdir` failed");
        closedir(dirp);
        exit(EXIT_FAILURE);
    }

    if (closedir(dirp) == -1) {
        perror("`closedir` failed");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
