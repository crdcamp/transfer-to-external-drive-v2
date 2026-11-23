# Logic Plan

You need to come up with a concise logical plan for determining **if a file is in the source, but not in the target**, or **if a file is in the target, but not in the source**. A **file can only meet one of those two criteria**. TRUE/FALSE, YES/NO. SIMPLE!

**Last resort idea:** A metadata array of both children linked lists *could* make this logic easier to implement, but that will add overhead that you could avoid if you **really** think through how to do this, since it requires walking the children linked list upon its creation. Since you're passing strings around and not opening and closing file paths, it wouldn't add that much to the actual file processing.

 Another thing worth considering: **There are no duplicates in this problem**. No OS will allow a duplicate file in the same directory!!! Moreover, the files are sorted. So, that actually makes the problem a bit easier!

 ## Some final important notes before beginning full logic

 **In regards to the number of files in the source and target directories:**
 * The source directory can be larger than the target directory.
 * The target directory can be larger than the source
 * The source and target directories can be the same size.

 **In regards to source files not being in the target directory:**
 * When comparing a source and target file, if the current source and target files don't match, and the next source file doesn't match the current target file, that implicitly means that the source file is not in the target directory.

 **In regards to the target files not being in the source directory:**
 * I *think* the above logic holds true. Definitely double check this claim.

 **VERY IMPORTANT:** Pay **very close attention** to your decisions between continuing to the next source file iteration, continuing to the next target file iteration, or continuing both iterations.

 # Larger Source than Target

## Source file is not in the target directory

```
source: a  b  c
target: a  c
```

**source = `a`, target = `a`**
* Ignoring the additional logic needed (modification date and size comparisons), we simply continue to the next file

**source = `b`, target = `c`**
* The files are not the same. What do we do?
* Because there are no duplicates and the lists are sorted, we can check the next file in `source`. If the next file in source does not match the current target file, that means that the source file is not in the target directory (again, because the lists are sorted and there are no duplicates). **Don't forget to check if source->next is `NULL`**.
* The next file in `source` is `c`. The current file in `target` is also `c`.
* Therefore, the source file `c` is the the target directory.
* This (I'm pretty sure) implicitly means that the source file `b` is not in the target directory. Therefore, we copy source file `b` over to the target directory. This brings up a question that will likely be answered later: **should we only copy and delete after all source and target file boolean checks are complete?** I'd say.... most likely.

**source = `c`, target = `NULL`**
* We have reached the end of the target list while the source list has an **unknown** number of entries left (since we do not know the size of either list).
* This means that the rest of the files in the source directory are not in the target directory.
* Therefore, we stop iterating over the target.
* We then continue iterating over the source and copy files over to the target until `source` == `NULL`.
* Since we are at the end of the target, we do not need to worry about checking the target files anymore, and can blindly copy over the rest of source files

## Target file is not in the source directory

```
source: a  b  c  d  e
target: a  b  c  f
```

**source = `a`, target = `a`**
* Ignoring other logic, continue

**source = `b`, target = `b`**
* Ignoring other logic, continue

**source = `c`, target = `c`**
* Ignoring other logic, continue

**source = `d`, target = `f`**
* Similar to before, we check the next value in `source` (assuming it's not `NULL`).
* The next value in `source` is `e`. The current value in `target` is `f`.
* This implicitly means that target value `f` is not in the source directory.
* Delete target value `f`.

*This could be wrong.*

# Larger Target than Source

## Source file is not in the target directory

```
source: a  b  e
target: a  b  c  d
```

**source = `a`, target = `a`**
* Ignoring other logic, continue

**source = `b`, target = `b`**
* Ignoring other logic, continue

**source = `e`, target = `c`**
* The files don't match! What do we do?
* The next file in the source is `NULL`. So, we can't use the next source value check here.
* The next value in the target directory is `d`. I'm... not sure how to work things out from here.

Know what? Let's start coding and see if we can figure this out.

```
source: a  b  g
target: a  c
```

# List Alignment

In the current iteration of the code, where we're trying to figure out how to move the linked lists after detecting non-matching directories, we have the following code block:

```c
if (source_file_is_a_directory &&
    target_file_is_a_directory &&
    !file_names_are_same) {

    printf("Source directory `%s` and...\ntarget directory `%s`\n...are directories and do not have matching names. Checking next source file for match...\n",
        source_full_path, target_full_path);

    // If the next file in the source list is not null, is a directory, and has the same name as the current item in the target list,
    // that means that the current source directory file is not in the target directory, so copy it over
    if ((source_ptr->fts_link != NULL) &&
        (is_a_directory(source_ptr->fts_link)) &&
        (strcmp(source_ptr->fts_link->fts_name, target_ptr->fts_name) == 0)) {
        source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
        target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, "");

        printf("Next source directory `%s` matches current target file `%s`. This means that current source directory `%s` is not in the target directory. Copying current source file `%s to target `%s`\n",
        source_ptr->fts_link->fts_name, target_ptr->fts_name, source_ptr->fts_name, source_full_path, target_full_path);

        //directory_copy(source_full_path, target_full_path);
        printf("Moving source from directory `%s` to directory ", source_ptr->fts_name);
        source_ptr = source_ptr->fts_link;
        printf("`%s`\n", source_ptr->fts_name);
        printf("Current target directory: %s\n\n", target_ptr->fts_name);
        continue;
    }
```

In our code directories, we have the following source and target:

**Source:**
```
SubDir1  SubDir2  SubDir4  SubDir6
```

**Target:**
```
SubDir1  SubDir2  SubDir3  SubDir4
```

The relevant portion of the current code outputs the following:

```
Source directory `TestDirs/Source/SubDir4` and...
target directory `TestDirs/Target/SubDir3`
...are directories and do not have matching names. Checking next source file for match...
Next source directory `SubDir6` does not match the current target directory `SubDir3`. This means that the next source directory is not in the target directory. Copying next source file `TestDirs/Source/SubDir6` to target `TestDirs/Target/

Moving target from directory `SubDir3` to directory `SubDir4`
Current source directory: SubDir4

Source `TestDirs/Source/SubDir4` and...
target `TestDirs/Target/SubDir4`
...are directories and have matching names. Moving to next list item
```

Keep in mind that the lists **do not change** when you copy or delete files. This is because the linked list of children is created before another read to a directory is made. Therefore, **any worries about copying and deleting files affecting the list alignment are out the window**.

As the above output states, the

"Next source directory `SubDir6` does not match the current target directory `SubDir3`. This means that the next source directory is not in the target directory. Copying next source file `TestDirs/Source/SubDir6` to target `TestDirs/Target/"

Now, **how do we properly move to the next files in one or both linked lists?**

Well, when we

```
Moving target from directory `SubDir3` to directory `SubDir4`
```

... and

```
Current source directory: SubDir4
```

We've just **ended up right back where we started** at `SubDir4`. What does this mean? Well, it obviously means that **you have not moved the pointer far enough!**

Moreover, this indicates we need a string comparison in a while loop to get us where we need to be!

THEREFORE, we actually want to move the current source directory forward, and move the target directory forward until its `strcmp` with a `> 0` condition with the *new* current source directory (I'm pretty sure)!

Go to the code and try this NOW.
