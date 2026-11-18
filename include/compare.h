#ifndef COMPARE_H
#define COMPARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include "arena.h"

FTSENT *children_get_list(FTS *handle);
void children_print_list(FTSENT *children_list);
void *children_sort_list(Arena *arena_scratch, FTSENT *children_list);
void compare_directories(char **source, char **target);

#endif
