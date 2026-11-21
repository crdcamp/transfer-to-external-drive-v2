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
 * When comparing a source and target file, if the current source and target files don't match, and the

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
* Since we are at the end of the target, we do not need to worry about checking the target files anymore, and can blindly copy over the source files

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
source: a  b  c  d
target: a  b  d  e  f
```

**source = `a`, target = `a`**
* Ignoring other logic, continue

**source = `b`, target = `b`**
* Ignoring other logic, continue

**source = `c`, target = `d`**
* The files don't match! What do we do?
* Assuming the next source value is not `NULL`, we check the next value in `source`
* The next value in `source` is `d`.
* Therefore, source file `d` is in the target directory. Move on to the next source file and the next target file (again, assuming they are not `NULL`).

**source = `d`, target = `e`**
* The files don't match! Ahhh what do we do?!
* If the next file in source is not `e`, then that means source file `e` in not in the target directory.
* In this case, the next source file is `NULL`. Therefore, source file `e` is not in the target directory.
* Delete target directory file `e`.

## Target file is not in the source directory

# Source and Target are the Same Size

# Putting it All Together
