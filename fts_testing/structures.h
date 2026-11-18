#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>

// "Directory entries"
#include <dirent.h>

// `fts` (file tree stream) functionality
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include <stdlib.h>
#include <time.h>

#include <copyfile.h>

typedef struct FTSNode {
    char *path;            // fts_path Full file path
    char *name;            // fts_name File name
    struct _ftsent *parent; // fts_parent Parent directory name
    short pathlen;         // fts_pathlen Size in bytes of the file's name
    short level;           // fts_level How deep within the directory hierarchy the file is
    void *pointer;         // fts_pointer A NULL pointer that may or may not come in handy
    time_t last_mod;       // fts_statp->st_mtime Time since last modification. Will be used for determining whether or not to overwrite a file
} FTSNode;

void fts_print_entry_info(FTSENT *node);
FTSENT *get_children_list(FTS *handle);
void fts_compare_dirs(char **source, char **target);

#endif

//FTSENT
// typedef struct _ftsent {
//     unsigned short fts_info;     /* flags for FTSENT structure */
//     char          *fts_accpath;  /* access path */
//     char          *fts_path;     /* root path */
//     short          fts_pathlen;  /* strlen(fts_path) */
//     char          *fts_name;     /* filename */
//     short          fts_namelen;  /* strlen(fts_name) */
//     short          fts_level;    /* depth (-1 to N) */
//     int            fts_errno;    /* file errno */
//     long           fts_number;   /* local numeric value */
//     void          *fts_pointer;  /* local address value */
//     struct ftsent *fts_parent;   /* parent directory */
//     struct ftsent *fts_link;     /* next file structure */
//     struct ftsent *fts_cycle;    /* cycle structure */
//     struct stat   *fts_statp;    /* stat(2) information */
// } FTSENT;

// STAT
// struct stat {
//     dev_t     st_dev;     /* ID of device containing file */
//     ino_t     st_ino;     /* inode number */
//     mode_t    st_mode;    /* protection */
//     nlink_t   st_nlink;   /* number of hard links */
//     uid_t     st_uid;     /* user ID of owner */
//     gid_t     st_gid;     /* group ID of owner */
//     dev_t     st_rdev;    /* device ID (if special file) */
//     off_t     st_size;    /* total size, in bytes */
//     blksize_t st_blksize; /* blocksize for file system I/O */
//     blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
//     time_t    st_atime;   /* time of last access */¡
//     time_t    st_mtime;   /* time of last modification */
//     time_t    st_ctime;   /* time of last status change */
// };
