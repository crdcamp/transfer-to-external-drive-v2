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

// REWRITE SECTION
// For error printing (this can almost certainly be done without defining this struct)
static const char *FTS_INFO_NAME(unsigned short info) {
    switch (info) {
        case FTS_D:       return "FTS_D";        // directory (preorder)
        case FTS_DC:      return "FTS_DC";       // directory causing a cycle
        case FTS_DEFAULT: return "FTS_DEFAULT";  // other file type
        case FTS_DNR:     return "FTS_DNR";      // unreadable directory
        case FTS_DOT:     return "FTS_DOT";      // . or ..
        case FTS_DP:      return "FTS_DP";       // directory (postorder)
        case FTS_ERR:     return "FTS_ERR";      // generic error
        case FTS_F:       return "FTS_F";        // regular file
        case FTS_NS:      return "FTS_NS";       // stat failed
        case FTS_NSOK:    return "FTS_NSOK";     // stat not requested
        case FTS_SL:      return "FTS_SL";       // symlink
        case FTS_SLNONE:  return "FTS_SLNONE";   // dangling symlink
        default:          return "UNKNOWN";
    }
}

FTSENT *skip_apple_files(FTSENT *entry);
bool is_a_directory_empty(FTSENT *entry);
bool is_a_symlink(FTSENT *entry);
void test_directories_sync(Arena *arena_scratch, char *source_directory, char *target_directory);

#endif
