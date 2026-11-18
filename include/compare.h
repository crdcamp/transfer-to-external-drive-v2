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
int by_name(const FTSENT **a, const FTSENT **b);
void compare_directories(Arena *arena_scratch, char **source, char **target);

#endif
