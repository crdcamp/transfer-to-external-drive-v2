#ifndef TRANSFER_H
#define TRANSFER_H

// Credit: https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir/
#include <stdio.h>
#include <dirent.h>

typedef struct Dirent {
    ino_t d_ino; // Inode number (unique identifier for the file)
    char d_name[]; // Null-terminated filename (variable-length array)
    // Optional: unsigned char d_type;  // File type (POSIX extension)
} Dirent;

// Initializes a directory stream (an iterator) for a given directory path.
// Parameters: path (path to the directory to open).
// Returns: A DIR* (directory stream pointer) on success; NULL on failure (check errno for details, e.g., ENOENT for "directory not found").
// Usage: Always check the return value—failing to open a directory (e.g., due to permissions) is common.
DIR *open_directory(const char *path);

// Reads the next entry from an open directory stream.
// Parameters: path (path to the directory to open).
// Returns: A DIR* (directory stream pointer) on success; NULL on failure (check errno for details, e.g., ENOENT for "directory not found").
// Usage: Always check the return value—failing to open a directory (e.g., due to permissions) is common.
Dirent read_directory(DIR *directory_stream_pointer);

// Cleans up the directory stream, freeing resources.
// Parameters: dirp (directory stream to close).
// Returns: 0 on success; -1 on failure (check errno).
// Usage: Always call closedir() after finishing with a directory stream to prevent resource leaks.
int close_directory(DIR *directory_stream_pointer);

#endif
