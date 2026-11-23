* Test each copy and delete function on large directories
* Revisit error handling. You need to get this right in order to continue. This turns out to be a step that you cannot skip.
* Look at `remove` documentation. Ensure your `remove` function works on any and every directory in existence.
* Define top priorities of fixes when you have a better picture of what to do.
* Review `copyfile` documentation. Ensure you're using it correctly.
* Maybe add a slash option for file_path_concatenate? Or maybe an "add slash?" option for file/directory copying?
* Write down every single logic consideration that crosses your mind in `all_logical_considerations.md`.
* Test every single delete and copy function with your actual directories before EVEN THINKING about using them in the recursive function
* When you rewrite the recursive function, rewrite it with the recursion in place from the start.
* Break down every file path concatenation
* When you rewrite the sync function, ensure that the junk files are implemented into it, rather than having them as a separate function.

# Error Handling

## `children_get_list()`

The first function we're revisiting is `children_get_list()` in [`sync.c`](../src/sync.c) and the ambiguity this function causes due to its improper error handling. Luckily, we've found an `fts` function in [the documentation](https://linux.die.net/man/3/fts_children) specifically made for this - `fts_errno`.

As a refresher, `fts_children` and its relevant fields are as follows:

```c
FTSENT *fts_children(FTS *ftsp, int options);
```

**Error handling with `fts_children` can be done with `fts_errno`.**

Upon return of a `FTSENT` structure from the `fts_children()` or `fts_read()` functions, with its `fts_info` field set to `FTS_DNR`, `FTS_ERR` or `FTS_NS`, the `fts_errno` field contains the value of the external variable errno specifying the cause of the error. Otherwise, the contents of the fts_errno field are undefined.

**Let's clarify what each of these `fts_info` fields mean to get `fts_errno`:**

`FTS_DNR`

A directory which cannot be read. This is an error return, and the fts_errno field will be set to indicate what caused the error.

`FTS_ERR`

This is an error return, and the `fts_errno` field will be set to indicate what caused the error.

`FTS_NS`

A file for which no stat(2) information was available. The contents of the fts_statp field are undefined. This is an error return, and the fts_errno field will be set to indicate what caused the error.

Using this in combination with `errno.h` can help ensure that we're not getting errors:

```c
FTSENT *test_children_get_list(FTS *handle) { // Assumes `fts_open` was used prior
    errno = 0; // Always set to 0 in a function, since errno can be edited by other operations
    FTSENT *list_head = fts_read(handle); // Handle has the be read before being passed to `fts_children`
    FTSENT *children_list = fts_children(handle, 0);
    if (errno != 0) {
        printf("Error : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return children_list;
}
```

Now, any time FTS_DNR, FTS_ERR or FTS_NS are found in the `fts_info` field, the program will throw an error and exit.

This also brings up the concern of file processing that fails part of the way through. We certainly shouldn't give this script `sudo` permissions to address the error of files that require permissions. That wouldn't address `FTS_DNR` (a file that cannot be read) or `FTS_NS` (a file with no stats - a highly unlikely instance).

This could be resolved by creating sort of tree structure containing all the relevant commands, and recursing through that tree to execute all commands, but that doesn't fully resolve the issue. How about this? You can worry about cancelled operations in a project improvement. After all, that's not a huge concern at the moment.

**Implementing source empty check**

With this error handling now implemented into the main code, we now have to figure out how to address an empty source directory and **only** the source directory. It doesn not matter if the target is empty.

We cannot check if the source is empty in `directories_sync`, since this function will be recursive. At some point, an empty source directory could be passed into this function.

Therefore, the check has to happen somewhere outside of the function. I believe the only solution here is a second `fts_open` before the `fts_open` in `directories_sync()`. So, let's sort that out.

**Or...** is this simply unnecessary? After some consideration, I do not believe this check is needed, given that if the source is empty simply nothing happens. However, a single additional `fts_open` would be incredibly minimal, since this wouldn't be within any loops or recursions.

**Final conclusion:** This implementation could avoid dumb mistakes and future headaches, with an incredibly small, pretty insignificant performance hit. However, trying to implement it is annoying, and not quite worth the effort. Skipping for now.

Let's move on to other instances of error handling that need addressing. The next function in order of how they're defined is [`file_copy()` in sync.c](../src/sync.c)

## `file_copy()`

While I'm pretty sure the error handling is fine here, it's worth double checking the docs.

The function in its original form is as follows:

```c
// Copy a single file
void file_copy(const char *source, const char *target) {
    if (copyfile(source, target, NULL, COPYFILE_DATA | COPYFILE_STAT) != 0) {
        perror("copyfile");
        exit(EXIT_FAILURE);
    }
    return;
}
```

First thing I noticed are the flags `COPYFILE_DATA | COPYFILE_STAT`.

`COPYFILE_DATA` copies the source file's data, while `COPYFILE_STAT` copies the file's POSIX information. I was originally going to change this to `COPYFILE_ALL`, but I think this would result in portability issues, since `COPYFILE_ALL` copies *everything*, including all of Apple's little quirks. So, we'll leave that part of the function as is.

**There are some noteworthy things for copying into a directory:**

"Copying into a directory

Copying files into a directory is supported. If to is a directory, from will be copied into to. (If from is a directory, copying its contents requires use of the COPYFILE_RECURSIVE parameter, which is documented below.)"

Given that this is for only single files, we're only copying from a file to a directory. **This does, however**, bring up some needed double checking of `directory_copy`. **Keep a mental note of that**.

`copyfile` also returns an `errno`, and the function checks for that. So, I think we're all good here.

Just need to write some quick tests then we should certainly check out `directory_copy()`, as that also uses `copyfile`.

Alright. With some basic tests, `file_copy()` works as intended. You can find the path formulation for it in [`file_path_concatenations.c`](notes/file_path_concatenations.md).

For now, the only change I want to make was a different error print:

```c
void test_directory_copy(const char *source, const char *target) {
    copyfile_flags_t flags = COPYFILE_DATA | COPYFILE_STAT | COPYFILE_RECURSIVE;

    if (copyfile(source, target, NULL, flags) != 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}
```

Now I'm just going to implement `strerror` into the error handling to make errors a bit better.

## `directory_copy()`

The original function is as follows:

```c
// Recursively copy a directory and all of its contents. Overwrites if names are the same
void directory_copy(const char *source, const char *target) {
    copyfile_flags_t flags = COPYFILE_DATA | COPYFILE_STAT | COPYFILE_RECURSIVE;

    if (copyfile(source, target, NULL, flags) != 0) {
        perror("copyfile");
        exit(EXIT_FAILURE);
    }
    return;
}
```

As a first note, **this should only be used when copying the source to target**. Obvious, I know, but still...

### **Return to this test when recursion has been implemented into `test_directories_sync()`!!!**

The only way to continue is by testing this function and seeing if we run into any issues. You **need to be careful here when considering how this will behave when in the recursive loop**.


# Implementing the Recursion Before Continuing

Now time for the hardest part. God damnit...

Before showing the code, note that:
* I implemented `skip_apple_files`
* The arena rewinding is not working as intended

Here's the code so far:

```c
void test_directories_sync(Arena *arena_scratch, char *source_directory, char *target_directory) {
    if (source_directory == NULL || target_directory == NULL) return;
    size_t checkpoint = arena_checkpoint(arena_scratch);

    char *source_directory_fts[] = {source_directory, NULL};
    char *target_directory_fts[] = {target_directory, NULL};

    // Not entirely sure if `fts_open` needs error handling here
    FTS *source_handle = fts_open(source_directory_fts, FTS_PHYSICAL, by_name);
    if (source_handle == NULL) return;
    FTS *target_handle = fts_open(target_directory_fts, FTS_PHYSICAL, by_name);
    if (target_handle == NULL) return;

    FTSENT *source_children = test_children_get_list(source_handle);
    FTSENT *target_children = test_children_get_list(target_handle);

    FTSENT *s = source_children;
    FTSENT *t = target_children;
    while (s != NULL || t != NULL) {
        // DIRECTORY COPY TEST
        // If `target_ptr` is NULL, that means all the remaining source files are not in the target directory
        // So, we copy the rest of the files in the current source directory to the target directory
        // Advance only `source_ptr
        if (t == NULL) {
            // Skip apple file
            s = skip_apple_files(s);
            while (s != NULL) {
                char *source_path = file_path_concatenate(arena_scratch, s->fts_path, s->fts_name);
                if (is_a_directory(s)) {
                    char *target_path = file_path_concatenate(arena_scratch, target_directory, "");
                    printf("Copying source directory: `%s`\n", source_path);
                    printf("Into target:              `%s`\n", target_path);
                    //test_directory_copy(source_directory, target_directory);
                }
                else if (is_a_file(s)) {
                    char *target_path = file_path_concatenate(arena_scratch, target_directory, s->fts_name);
                    printf("Copying source file `%s`\n", source_path);
                    printf("Into target:        `%s`\n", target_path);
                    //file_copy(source_path, target_path);
                }
                s = s->fts_link;
            }

            break;
        }
        // If `source_ptr` is NULL, that means all remaining target files are not in the source directory
        // So, we delete the rest of the files in the current target directory
        // Advance only `target_ptr`
        if (s == NULL) {
            t = skip_apple_files(t);
            while (t != NULL) {
                char *target_path = file_path_concatenate(arena_scratch, t->fts_path, t->fts_name);
                if (is_a_directory(t)) {
                    printf("Deleting file `%s`\n", target_path);
                    //directory_delete(target_path);
                }
                else if (is_a_file(t)) {
                    printf("Deleting file `%s`\n", target_path);
                    //file_delete(target_path);
                }
                t = t->fts_link;
            }
        }
        break;
    }

    // Cleanup
    printf("Arena after rewind: ");
    print_arena_info(arena_scratch);
    fts_close(source_handle);
    fts_close(target_handle);
    arena_rewind(arena_scratch, checkpoint);
}
```

This code is currently fixated on what happens if the source is populated, and the target has nothing in it.

I believe that, in order to get the recursion working (ignoring the arena rewinding for now), that function needs to call itself in every logic branch. This is going to be relatively easy, but the hard part is getting the file names to pass to the next recursion without hitting a segmentation fault.

The seg fault results from attempting to access `NULL` `fts_link`s. And I'm having trouble figuring out how to address this.

The core logic that's here right now happens when either the source or target child list of the current directory depth is at its end.

By extension, the problem I don't know how to address is passing whichever list is at its end (null) to the next recursion. **I think a big confusion is stemming from not thinking about the while loop as an iteration over the current depth**.

# DO NOT START UNTIL ERROR HANDLING IS DONE: Removing Apple Files within the recursion
