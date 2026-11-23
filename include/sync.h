#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <copyfile.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <stdbool.h>

#include <errno.h>

#include "arena.h"

// `fts` operations
FTSENT *children_get_list(FTS *handle);
int by_name(const FTSENT **a, const FTSENT **b);
// File operations
char *file_path_concatenate(Arena *arena_scratch, char *base_path, char *file_name);
void file_copy(const char *source, const char *target);
void file_delete(char *file_path);
void directory_copy(const char *source, const char *target);
void directory_delete(char *file_path);
// Sync operations
bool is_a_directory(FTSENT *entry);
bool is_a_file(FTSENT *entry);
void directories_sync(Arena *arena_scratch, char *source, char *target);

// TEMPORARY REWRITE FUNCTIONS
FTSENT *skip_apple_files(FTSENT *entry);
FTSENT *test_children_get_list(FTS *handle);
void test_directories_sync(Arena *arena_scratch, char *source_directory, char *target_directory);
void test_file_copy(const char *source, const char *target);
void test_directory_copy(const char *source, const char *target);

#endif
