#ifndef TRANSFER_H
#define TRANSFER_H

// Credit: https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir/
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

typedef struct Dirent {
    ino_t d_ino; // Inode number (unique identifier for the file)
    char d_name[]; // Null-terminated filename (variable-length array)
    // Optional: unsigned char d_type;  // File type (POSIX extension)
} Dirent;

#endif
