You need to come up with a concise logical plan for determining **if a file is in the source, but not in the target**, or **if a file is in the target, but not in the source**. A file can only meet one of those two criteria. You'll want to make a directory structure for testing this.

You also might want to consider converting the logic for "if it's in source, but not in target" and "if it's in target, but not in source" into  function to make the coding a bit easier.

A metadata array of both children linked lists could make this logic easier to implement, but that will add overhead that you could avoid if you **really** think through how to do this, since it requires walking the children linked list upon its creation. Since you're passing strings around and not opening and closing file paths, it wouldn't add that much to the actual file processing.

With all those ideas out of the way, this is the main condition your logic needs to handle. If this happens in your directory structure, how do you walk the linked lists to handle this? Don't just start typing a think through, **actually think** about it. There is certainly a way to compare these lists without walking them beforehand. After all, they're already sorted!
 * **Consider doing various internet searches for comparing sorted linked lists**.
    * Update: The only results I found were very basic and cannot be used to answer this question.
 * **Start by creating a very simple test linked list like the one below**. You don't have to go straight to the actual implementation to understand the basis of how it'd work.
 * **Another thing worth considering:** There are no duplicates in this problem. No OS will allow a duplicate file! SO, that actually makes the problem a bit easier!

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

# Larger Target than Source
