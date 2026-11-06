#ifndef TRANSFER_H
#define TRANSFER_H

// Credit: https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir/
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

void dirs_list_all(const char *file_path);

#endif
