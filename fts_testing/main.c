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

void fts_testing(char **file);
void fts_print_entry_info(FTSENT *node);
void copyfile_testing(char *file);


int main(void) {
    // `fts` requires the array of paths to end in NULL so it can terminate
    // without a length parameter
    char *paths[] = {"TestDirs", NULL};
    fts_testing(paths);

    return 0;
}

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
//     time_t    st_atime;   /* time of last access */
//     time_t    st_mtime;   /* time of last modification */
//     time_t    st_ctime;   /* time of last status change */
// };

void fts_print_entry_info(FTSENT *node) {
    struct _ftsent;
    printf("Full path:           %s\n", node->fts_path);
    printf("Name:                %s\n", node->fts_name);
    if (strcmp(node->fts_parent->fts_name, "") == 0) {
        printf("Parent name:         %s\n", "No parent");
    }
    else {
        printf("Parent name:         %s\n", node->fts_parent->fts_name);
    }
    printf("Path length:         %i\n", node->fts_pathlen);
    printf("Depth:               %i\n", node->fts_level);
    printf("Pointer:             %p\n", node->fts_pointer);
    printf("Last modification:   %s", ctime(&node->fts_statp->st_mtime));
}

// DON'T FORGET WE ALSO HAVE `fts_children` to test out!
void fts_testing(char **paths) {
    // Returns a "handle" on the file hierarchy, which is then supplied
    // to the other `fts` functions

    // The function `fts_read()` returns a pointer to a structure describing one of the files in the file hierarchy.

    // The function fts_children() returns a pointer to a linked list of structures,
    // each of which describes one of the files contained in a directory in the hierarchy
    // NOTE: You probably shouldn't use this for the purposes of this project

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
    FTSENT *node;
    while ((node = fts_read(fts_handle)) != NULL) {
        if (node->fts_info == FTS_D) {
            fts_print_entry_info(node);
            printf("\n");
        }

        if (node->fts_info == FTS_F) {
            fts_print_entry_info(node);
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

    // Now... before you go nuts with the coding, you'll need to read through all of the
    // `fts` docs, as there might be some tools in there you'll need. Also make sure
    // to fully read over both the structs just so you're not forgetting any useful info
    // that you may need for the structs as well

    // NOTE: There's an `fts_pointer` field, which can easily be used for getting information
    // more quickly. This might come in handy

    // NOTE: You'll need a way to create directories. You can compare both file structures until
    // they aren't the same anymore. Once you reach one that doesn't match, you overwrite newer files with
    // older files, delete files not found in the source directory, and make directories that exist in the
    // source directory, but not the target directory
}
