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

// `fts` operations
FTSENT *children_get_list(FTS *handle);
int by_name(const FTSENT **a, const FTSENT **b);
// File operations
char *file_path_concatenate(Arena *arena_scratch, char *base_path, char *file_name);
void file_copy(char *source, char *target);
void file_delete(char *file_path);
void directory_copy(char *source, char *target);
void directory_delete(char *file_path);
// Main
void sync_directories(Arena *arena_scratch, char *source, char *target);

#endif
