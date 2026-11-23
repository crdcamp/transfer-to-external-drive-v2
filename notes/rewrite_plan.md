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

# `fts_children` Error Handling

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
