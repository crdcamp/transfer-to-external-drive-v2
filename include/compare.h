#ifndef COMPARE_H
#define COMPARE_H

#include <stdio.h>

// "Directory entries"
#include <dirent.h>

// File tree stream functionality
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include <stdlib.h>
#include <time.h>

FTSENT *children_get_list(FTS *handle);
void children_print_list(FTSENT *children_list);
void *children_sort_list(FTSENT *children_list);
void compare_directories(char **source, char **target);

#endif
