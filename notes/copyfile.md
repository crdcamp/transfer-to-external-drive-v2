# COPYFILE(3)

*Library Functions Manual — macOS 15.8, March 29, 2024*

## NAME

`copyfile`, `fcopyfile`, `copyfile_state_alloc`, `copyfile_state_free`, `copyfile_state_get`, `copyfile_state_set` – copy a file

## LIBRARY

Standard C Library (libc, `-lc`)

## SYNOPSIS

```c
#include <copyfile.h>

int
copyfile(const char *from, const char *to, copyfile_state_t state,
    copyfile_flags_t flags);

int
fcopyfile(int from, int to, copyfile_state_t state,
    copyfile_flags_t flags);

copyfile_state_t
copyfile_state_alloc(void);

int
copyfile_state_free(copyfile_state_t state);

int
copyfile_state_get(copyfile_state_t state, uint32_t flag, void * dst);

int
copyfile_state_set(copyfile_state_t state, uint32_t flag,
    const void * src);

typedef int
(*copyfile_callback_t)(int what, int stage, copyfile_state_t state,
    const char * src, const char * dst, void * ctx);
```

## DESCRIPTION

These functions are used to copy a file's data and/or metadata. (Metadata consists of permissions, extended attributes, access control lists, and so forth.)

The `copyfile_state_alloc()` function initializes a `copyfile_state_t` object (which is an opaque data type). This object can be passed to `copyfile()` and `fcopyfile()`; `copyfile_state_get()` and `copyfile_state_set()` can be used to manipulate the state (see below). The `copyfile_state_free()` function is used to deallocate the object and its contents.

The `copyfile()` function can copy the named *from* file to the named *to* file; the `fcopyfile()` function does the same, but using the file descriptors of already-opened files. If the *state* parameter is the return value from `copyfile_state_alloc()`, then `copyfile()` and `fcopyfile()` will use the information from the state object; if it is `NULL`, then both functions will work normally, but less control will be available to the caller.

### Content flags

The *flags* parameter controls which contents are copied:

| Flag | Description |
|------|-------------|
| `COPYFILE_ACL` | Copy the source file's access control lists. |
| `COPYFILE_STAT` | Copy the source file's POSIX information (mode, modification time, etc.). Note that setuid and setgid bits are by default not copied from source files or to destination files that reside on volumes that are mounted with the `MNT_NOSUID` `mount(2)` flag, but this can be overridden through the use of `COPYFILE_STATE_PRESERVE_SUID` in the `copyfile_state_t` object. |
| `COPYFILE_XATTR` | Copy the source file's extended attributes. |
| `COPYFILE_DATA` | Copy the source file's data. |

These values may be or'd together; several convenience macros are provided:

| Macro | Description |
|-------|-------------|
| `COPYFILE_SECURITY` | Copy the source file's POSIX and ACL information; equivalent to `(COPYFILE_STAT\|COPYFILE_ACL)`. |
| `COPYFILE_METADATA` | Copy the metadata; equivalent to `(COPYFILE_SECURITY\|COPYFILE_XATTR)`. |
| `COPYFILE_ALL` | Copy the entire file; equivalent to `(COPYFILE_METADATA\|COPYFILE_DATA)`. |

### Behavior flags

The `copyfile()` and `fcopyfile()` functions can also have their behavior modified by the following flags:

- **`COPYFILE_RECURSIVE`**
  Causes `copyfile()` to recursively copy a hierarchy. This flag is not used by `fcopyfile()`; see [Recursive Copies](#recursive-copies) for more information.

- **`COPYFILE_CHECK`**
  Return a bitmask (corresponding to the *flags* argument) indicating which contents would be copied; no data are actually copied. (E.g., if *flags* was set to `COPYFILE_CHECK|COPYFILE_METADATA`, and the *from* file had extended attributes but no ACLs, the return value would be `COPYFILE_XATTR`.)

- **`COPYFILE_PACK`**
  Serialize the *from* file. The *to* file is an AppleDouble-format file.

- **`COPYFILE_UNPACK`**
  Unserialize the *from* file. The *from* file is an AppleDouble-format file; the *to* file will have the extended attributes, ACLs, resource fork, and FinderInfo data from the *to* file, regardless of the *flags* argument passed in.

- **`COPYFILE_EXCL`**
  Fail if the *to* file already exists. (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_NOFOLLOW_SRC`**
  Do not follow the *from* file, if it is a symbolic link. (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_NOFOLLOW_DST`**
  Do not follow the *to* file, if it is a symbolic link. (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_MOVE`**
  Unlink (using `remove(3)`) the *from* file. (This is only applicable for the `copyfile()` function.) No error is returned if `remove(3)` fails. Note that `remove(3)` removes a symbolic link itself, not the target of the link.

- **`COPYFILE_UNLINK`**
  Unlink the *to* file before starting. (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_CLONE_FORCE`**
  Clone the file instead. This is a force flag, i.e. if cloning fails, an error is returned. This flag is equivalent to `(COPYFILE_EXCL | COPYFILE_STAT | COPYFILE_XATTR | COPYFILE_DATA | COPYFILE_NOFOLLOW_SRC)`.

  - If cloning is successful, progress callbacks will not be invoked, nor will any setuid or setgid bits be copied to the destination.
  - There is no support for cloning directories: if a directory is provided as the source, an error will be returned.
  - Since this flag implies `COPYFILE_NOFOLLOW_SRC`, symbolic links themselves will be cloned instead of their targets.
  - ACLs will not be cloned unless `COPYFILE_ACL` is also passed.
  - (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_CLONE`**
  Try to clone the file instead. This is a best-try flag, i.e. if cloning fails, fall back to copying the file. This flag is equivalent to `(COPYFILE_EXCL | COPYFILE_STAT | COPYFILE_XATTR | COPYFILE_DATA | COPYFILE_NOFOLLOW_SRC)`.

  - If cloning is successful, progress callbacks will not be invoked, nor will any setuid or setgid bits be copied to the destination.
  - There is no support for cloning directories: if a directory is provided as the source and `COPYFILE_CLONE_FORCE` is not passed, this will instead copy the directory.
  - Since this flag implies `COPYFILE_NOFOLLOW_SRC`, symbolic links themselves will be cloned instead of their targets.
  - Recursive copying is supported; see [Recursive Copies](#recursive-copies).
  - ACLs will not be cloned unless `COPYFILE_ACL` is also passed.
  - (This is only applicable for the `copyfile()` function.)

- **`COPYFILE_DATA_SPARSE`**
  Copy a file sparsely. This requires that the source and destination file systems support sparse files with hole sizes at least as large as their block sizes. This also requires that the source file is sparse, and for `fcopyfile()` the source file descriptor's offset be a multiple of the minimum hole size. If `COPYFILE_DATA` is also specified, this will fall back to a full copy if sparse copying cannot be performed for any reason; otherwise, an error is returned.

- **`COPYFILE_NOFOLLOW`**
  A convenience macro, equivalent to `(COPYFILE_NOFOLLOW_DST | COPYFILE_NOFOLLOW_SRC)`.

- **`COPYFILE_RUN_IN_PLACE`**
  If the *src* file has quarantine information, add the `QTN_FLAG_DO_NOT_TRANSLOCATE` flag to the quarantine information of the *dst* file. This allows a bundle to run in place instead of being translocated.

- **`COPYFILE_PRESERVE_DST_TRACKED`**
  Preserve the `UF_TRACKED` flag at *to* when copying metadata, regardless of whether *from* has it set. This flag is used in conjunction with `COPYFILE_STAT`, or `COPYFILE_CLONE` (for its fallback case).

### Copying into a directory

Copying files into a directory is supported. If *to* is a directory, *from* will be copied into *to*. (If *from* is a directory, copying its contents requires use of the `COPYFILE_RECURSIVE` parameter, which is documented below.)

### State get/set flags

The `copyfile_state_get()` and `copyfile_state_set()` functions can be used to manipulate the `copyfile_state_t` object returned by `copyfile_state_alloc()`. In both functions, the *dst* parameter's type depends on the *flag* parameter that is passed in.

#### `COPYFILE_STATE_SRC_FD` / `COPYFILE_STATE_DST_FD`

Get or set the file descriptor associated with the source (or destination) file. If this has not been initialized yet, the value will be -2. The *dst* (for `copyfile_state_get()`) and *src* (for `copyfile_state_set()`) parameters are pointers to `int`.

#### `COPYFILE_STATE_SRC_FILENAME` / `COPYFILE_STATE_DST_FILENAME`

Get or set the filename associated with the source (or destination) file. If it has not been initialized yet, the value will be `NULL`.

- For `copyfile_state_set()`, the *src* parameter is a pointer to a C string (i.e., `char *`); `copyfile_state_set()` makes a private copy of this string.
- For `copyfile_state_get()`, the *dst* parameter is a pointer to a pointer to a C string (i.e., `char **`); the returned value is a pointer to the *state*'s copy, and must not be modified or released.

#### `COPYFILE_STATE_STATUS_CB`

Get or set the callback status function (currently only used for recursive copies; see below for details). The *src* parameter and the *dst* parameter are pointers to a function of type `copyfile_callback_t` (see above).

#### `COPYFILE_STATE_STATUS_CTX`

Get or set the context parameter for the status call-back function (see below for details). The *src* parameter and the *dst* parameter are opaque pointers (type `void *`).

#### `COPYFILE_STATE_QUARANTINE`

Get or set the quarantine information with the source file. The *src* parameter is a pointer to an opaque object (type `void *`).

#### `COPYFILE_STATE_COPIED`

Get the number of data bytes copied so far. (Only valid for `copyfile_state_get()`; see below for more details about callbacks.) If a `COPYFILE_CLONE` or `COPYFILE_CLONE_FORCE` operation successfully cloned the requested objects, then this value will be 0. The *dst* parameter is a pointer to `off_t` (type `off_t *`).

#### `COPYFILE_STATE_XATTRNAME`

Get the name of the extended attribute during a callback for `COPYFILE_COPY_XATTR` (see below for details). This field cannot be set, and may be `NULL`.

#### `COPYFILE_STATE_WAS_CLONED`

True if a `COPYFILE_CLONE` or `COPYFILE_CLONE_FORCE` operation successfully cloned the requested objects. The *dst* parameter is a pointer to `bool` (type `bool *`).

#### `COPYFILE_STATE_SRC_BSIZE` / `COPYFILE_STATE_DST_BSIZE` / `COPYFILE_STATE_BSIZE`

Get or set the copy blocksize associated with the source or destination file (or both). The caller must ensure that its environment does not restrict a memory allocation of this size. If the copy blocksize for the destination file is specified to be larger than the copy blocksize for the source file, it will be ignored. If this has not been initialized by the caller, the value will be 0. The *dst* parameter and the *src* parameter are pointers to `uint32_t` (type `uint32_t *`).

#### `COPYFILE_STATE_FORBID_CROSS_MOUNT`

Get or set the current setting for crossing mount points during a `COPYFILE_RECURSIVE` operation. By default, recursive copying descends into directories that have a different device number than the directory from which the descent began. The *dst* parameter and the *src* parameter are pointers to `bool` (type `bool *`).

#### `COPYFILE_STATE_NOCPROTECT`

Get or set the current setting for copying content-protection classes from the source to the destination file. The *dst* parameter and the *src* parameter are pointers to `uint32_t` (type `uint32_t *`).

#### `COPYFILE_STATE_PRESERVE_SUID`

Get or set the current setting for preserving the setuid and setgid bits on the destination file during a copy. (See the descriptions above for `COPYFILE_STAT` and `COPYFILE_CLONE` for details.) The *dst* parameter and the *src* parameter are pointers to `uint32_t` (type `uint32_t *`).

#### `COPYFILE_STATE_RECURSIVE_SRC_FTSENT`

Get any available file entry information during a recursive copy. If none is available, the value will be `NULL`. (Only valid for `copyfile_state_get()`.) The *dst* parameter is a pointer to a pointer to a `const FTSENT` (type `const FTSENT **`); the returned value is the *state*'s copy, and must not be modified or released.

#### `COPYFILE_STATE_FORBID_DST_EXISTING_SYMLINKS`

Get or set the current setting for forbidding copying over existing symlinks. By default, copying over an existing destination is allowed regardless of the type of file that already exists at the destination path. The *dst* parameter and the *src* parameter are pointers to `uint32_t` (type `uint32_t *`).

## Recursive Copies

When given the `COPYFILE_RECURSIVE` flag, `copyfile()` (but not `fcopyfile()`) will use the `fts(3)` functions to recursively descend into the source file-system object. It then calls `copyfile()` on each of the entries it finds that way.

If a call-back function is given (using `copyfile_state_set()` and `COPYFILE_STATE_STATUS_CB`), the call-back function will be called four times for each directory object, and twice for all other objects. (Each directory will be examined twice, once on entry — before copying each of the objects contained in the directory — and once on exit — after copying each object contained in the directory, in order to perform some final cleanup.)

The call-back function will have one of the following values as the first argument, indicating what is being copied:

| Value | Meaning |
|-------|---------|
| `COPYFILE_RECURSE_FILE` | The object being copied is a file (or, rather, something other than a directory). |
| `COPYFILE_RECURSE_DIR` | The object being copied is a directory, and is being entered. (That is, none of the filesystem objects contained within the directory have been copied yet.) When `COPYFILE_STATE_FORBID_CROSS_MOUNT` is set, the call-back function will be skipped for any encountered mount points. |
| `COPYFILE_RECURSE_DIR_CLEANUP` | The object being copied is a directory, and all of the objects contained have been copied. At this stage, the destination directory being copied will have any extra permissions that were added to allow the copying removed (by a final `COPYFILE_STAT` operation — note that this occurs even if the `COPYFILE_STAT` flag was not passed to the recursive copy). |
| `COPYFILE_RECURSE_ERROR` | There was an error in processing an element of the source hierarchy; this happens when `fts(3)` returns an error or unknown file type. (Currently, the second argument to the call-back function will always be `COPYFILE_ERR` in this case.) |

The second argument to the call-back function will indicate the stage of the copy, and will be one of the following values:

| Value | Meaning |
|-------|---------|
| `COPYFILE_START` | Before copying has begun. The third parameter will be a newly-created `copyfile_state_t` object with the call-back function and context pre-loaded. |
| `COPYFILE_FINISH` | After copying has successfully finished. |
| `COPYFILE_ERR` | Indicates an error has happened at some stage. If the first argument to the call-back function is `COPYFILE_RECURSE_ERROR`, then an error occurred while processing the source hierarchy; otherwise, it will indicate what type of object was being copied, and `errno` will be set to indicate the error. |

The fourth and fifth parameters are the source and destination paths that are to be copied (or have been copied, or failed to copy, depending on the second argument).

The last argument to the call-back function will be the value set by `COPYFILE_STATE_STATUS_CTX`, if any.

The call-back function is required to return one of the following values:

| Value | Meaning |
|-------|---------|
| `COPYFILE_CONTINUE` | The copy will continue as expected. |
| `COPYFILE_SKIP` | This object will be skipped, and the next object will be processed. (Note that, when entering a directory, returning `COPYFILE_SKIP` from the call-back function will prevent the contents of the directory from being copied.) |
| `COPYFILE_QUIT` | The entire copy is aborted at this stage. Any filesystem objects created up to this point will remain. `copyfile()` will return -1, but `errno` will be unmodified. |

The call-back function must always return one of the values listed above; if not, the results are undefined.

The call-back function will be called twice for each object (and an additional two times for directory cleanup); the first call will have a *stage* parameter of `COPYFILE_START`; the second time, that value will be either `COPYFILE_FINISH` or `COPYFILE_ERR` to indicate a successful completion, or an error during processing. In the event of an error, the `errno` value will be set appropriately.

### Recursive cloning

Recursive cloning is also supported with the `COPYFILE_CLONE` flag (but not the `COPYFILE_CLONE_FORCE` flag). A recursive clone operation invokes `copyfile()` with `COPYFILE_CLONE` on every entry found in the source file-system object. Because `copyfile()` does not allow the cloning of directories, a recursive clone will instead copy any directory it finds (while cloning its contents). As symbolic links may point to directories, they are not followed during recursive clones even if the source is a symbolic link. Additionally, because the `COPYFILE_CLONE` flag implies the `COPYFILE_EXCL` flag, recursive clones require a nonexistent destination.

### Restrictions and destination behavior

The `COPYFILE_PACK`, `COPYFILE_UNPACK`, `COPYFILE_MOVE`, and `COPYFILE_UNLINK` flags are not used during a recursive copy, and will result in an error being returned.

Note that if the source path ends in a `/` its contents are copied rather than the directory itself (like `cp(1)`). The behavior of a recursive copy on a directory hierarchy also depends on the contents of the destination. If the destination is a directory, the source directory (or its contents, if the source path ends in a `/`) will be copied into it. If the destination exists but is not a directory, and the source is a non-empty directory, the copy will fail; the exact error set depends on the flags provided to `copyfile()` initially.

## Progress Callback

In addition to the recursive callbacks described above, `copyfile()` and `fcopyfile()` will also use a callback to report data (e.g., `COPYFILE_DATA`) progress. If given, the callback will be invoked on each `write(2)` call. The first argument to the callback function will be `COPYFILE_COPY_DATA`. The second argument will either be `COPYFILE_PROGRESS` (indicating that the write was successful), or `COPYFILE_ERR` (indicating that there was an error of some sort).

The amount of data bytes copied so far can be retrieved using `copyfile_state_get()`, with the `COPYFILE_STATE_COPIED` requestor (the argument type is a pointer to `off_t`).

When copying extended attributes, the first argument to the callback function will be `COPYFILE_COPY_XATTR`. The other arguments will be as described for `COPYFILE_COPY_DATA`; the name of the extended attribute being copied may be retrieved using `copyfile_state_get()` and the parameter `COPYFILE_STATE_XATTRNAME`.

When using `COPYFILE_PACK`, the callback may be called with `COPYFILE_START` for each of the extended attributes first, followed by `COPYFILE_PROGRESS` before getting and packing the data for each individual attribute, and then `COPYFILE_FINISH` when finished with each individual attribute. (That is, `COPYFILE_START` may be called for all of the extended attributes, before the first callback with `COPYFILE_PROGRESS` is invoked.) Any attribute skipped by returning `COPYFILE_SKIP` from the `COPYFILE_START` callback will not be placed into the packed output file.

The return value for the data callback must be one of:

| Value | Meaning |
|-------|---------|
| `COPYFILE_CONTINUE` | The copy will continue as expected. (In the case of error, it will attempt to write the data again.) |
| `COPYFILE_SKIP` | The data copy will be aborted, but without error. |
| `COPYFILE_QUIT` | The data copy will be aborted; in the case of `COPYFILE_PROGRESS`, `errno` will be set to `ECANCELED`. |

While the *src* and *dst* parameters will be passed in, they may be `NULL` in the case of `fcopyfile()`.

Note that progress callbacks are not invoked when a clone is requested (e.g. `COPYFILE_CLONE`) unless the clone cannot be performed and a copy is performed instead.

## RETURN VALUES

Except when given the `COPYFILE_CHECK` flag, `copyfile()` and `fcopyfile()` return less than 0 on error, and 0 on success. All of the other functions return 0 on success, and less than 0 on error.

## WARNING

- When performing a recursive copy, if the source hierarchy changes while the copy is occurring, the results are undefined; however, an effort is made to detect this and return an error to the caller instead of proceeding.
- `fcopyfile()` does not reset the seek position for either source or destination. This can result in the destination file being a different size than the source file.
- Both `copyfile()` and `fcopyfile()` will temporarily mark an unwritable destination writable for the duration of the copy. After the copy is performed (except when the `COPYFILE_STAT` flag is passed in), an attempt is made to revert the destination's permissions to their starting state.

## EXAMPLES

```c
/* Initialize a state variable */
copyfile_state_t s;
s = copyfile_state_alloc();

/* Copy the data and extended attributes of one file to another */
copyfile("/tmp/f1", "/tmp/f2", s, COPYFILE_DATA | COPYFILE_XATTR);

/* Convert a file to an AppleDouble file for serialization */
copyfile("/tmp/f2", "/tmp/tmpfile", NULL, COPYFILE_ALL | COPYFILE_PACK);

/* Release the state variable */
copyfile_state_free(s);

/* A more complex way to call copyfile() */
s = copyfile_state_alloc();
copyfile_state_set(s, COPYFILE_STATE_SRC_FILENAME, "/tmp/foo");

/* One of src or dst must be set... rest can come from the state */
copyfile(NULL, "/tmp/bar", s, COPYFILE_ALL);

/* Now copy the same source file to another destination file */
copyfile(NULL, "/tmp/car", s, COPYFILE_ALL);
copyfile_state_free(s);

/* Remove extended attributes from a file */
copyfile("/dev/null", "/tmp/bar", NULL, COPYFILE_XATTR);
```

## ERRORS

`copyfile()` and `fcopyfile()` will fail if:

| Error | Condition |
|-------|-----------|
| `EINVAL` | An invalid flag was passed in with `COPYFILE_RECURSIVE`. |
| `EINVAL` | The *from* or *to* parameter to `copyfile()` was a `NULL` pointer. |
| `EINVAL` | The *from* or *to* parameter to `fcopyfile()` was a negative number. |
| `ENOMEM` | A memory allocation failed. |
| `ENOTSUP` | The source file was not a directory, symbolic link, or regular file. |
| `ENOTSUP` | `COPYFILE_CLONE_FORCE` was specified and file cloning is not supported. |
| `ENOTSUP` | `COPYFILE_DATA_SPARSE` was specified, sparse copying is not supported, and `COPYFILE_DATA` was not specified. |
| `ECANCELED` | The copy was cancelled by callback. |
| `EEXIST` | The *to* parameter to `copyfile()` already existed and was passed in with `COPYFILE_EXCL`. |
| `ENOENT` | The *from* parameter to `copyfile()` did not exist. |
| `EACCES` | Search permission is denied for a component of the path prefix for the *from* or *to* parameters. |
| `EACCES` | Write permission is denied for a component of the path prefix for the *to* parameter. |
| `EBADF` | `COPYFILE_RECURSIVE` was specified and a change in the source hierarchy was detected. |

In addition, both functions may set `errno` via an underlying library or system call.

## SEE ALSO

`getxattr(2)`, `listxattr(2)`, `setxattr(2)`, `acl(3)`

## HISTORY

The `copyfile()` API was introduced in Mac OS X 10.5.

## BUGS

Recursive copies do not honor hard links.

---

*macOS 15.8 — March 29, 2024*
