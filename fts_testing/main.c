#include <stdio.h>

// "Directory entries"
#include <dirent.h>

// `fts` (file tree stream) functionality
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include <stdlib.h>
#include <time.h>

#include <copyfile.h>

void fts_testing(char **file);
void fts_print_info(FTSENT *node);
void copyfile_testing(char *file);


int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *paths[] = {"TestDirs/TestSource", NULL};
    fts_testing(paths);

    return 0;
}

// STRUCTS FOR FSTAT INFO
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
//     time_t    st_atime;   /* time of last access */
//     time_t    st_mtime;   /* time of last modification */
//     time_t    st_ctime;   /* time of last status change */
// };

void fts_print_info(FTSENT *node) {
    struct _ftsent;
    printf("path:              %s\n", node->fts_path);
    printf("parent name:       %s\n", node->fts_parent->fts_name);
    printf("name:              %s\n", node->fts_name);
    printf("path length:       %i\n", node->fts_pathlen);
    printf("Last modification: %s\n", ctime(&node->fts_statp->st_mtime));
    // Print a list of all the files within the directory (then use this in `fts_testing`)

}

// DON'T FORGET WE ALSO HAVE `fts_children` to test out!
void fts_testing(char **paths) {
    // Returns a "handle" on the file hierarchy, which is then supplied
    // to the other `fts` functions
    // I believe the final argument `compar` can be used to compare files, or
    // maybe even hierarchies!

    // Two structures are defined (and typedef'd) in the include file <fts.h>.
    // The first is FTS, the structure that represents the file hierarchy itself.
    // The second is FTSENT, the structure that represents a file in the file hierarchy.
    // Normally, an FTSENT structure is returned for every file in the file hierarchy.

    // Open the base path
    FTS *fts_handle = fts_open(paths, FTS_PHYSICAL, NULL);
    if (fts_handle == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Traverse all files and directories found in the base path
    struct _ftsent;
    FTSENT *node;
    while ((node = fts_read(fts_handle)) != NULL) {
        if (node->fts_info == FTS_F) {
            fts_print_info(node);
            printf("\n");
        }
    }
    fts_close(fts_handle);


    // Now we need to clarify exactly how we can use this functionality in one of
    // your personally created trees
    // Should we store the file path? Probably not, that would be making the same
    // mistake as before. There has to be a better way

    // You're greatly overthinking this. You can create a custom struct that is strongly
    // based on the `fts` structs and copy over the `fts` information onto those using the arena
    // `memcpy` solves most of these concerns anyway. Keep it simple to start by using just one
    // arena between two file paths. You can think about editing the code later to create one
    // arena per directory structure.
}
