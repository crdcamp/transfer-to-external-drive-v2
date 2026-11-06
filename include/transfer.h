#ifndef TRANSFER_H
#define TRANSFER_H

// Credit: https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir/
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int dirs_list_all(const char *file_path);
void directory_traverse(const char *file_path, int indent);

#endif
