#ifndef SYNC_H
#define SYNC_H

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
int by_name(const FTSENT **a, const FTSENT **b);
void sync_directories(Arena *arena_scratch, char **source, char **target);

#endif
