# Where You Left Off

`tree.c`- line 112: Replace `asprintf` with an arena implementation.

## Next Steps
* `tree.c`- `new_node` function: Convert `malloc` to an arena allocation
* `arena.c`: Think now it's a good time to implement a dynamic arena allocator
* `tree.c` - `tree_fill`: Add file name with spaces handling
* `tree.c`- `tree_fill` function: Implement file paths into the tree structure
  * Once that's done- try to make it it's own function
* `tree.c`- `tree_fill` function: Add arena expansion during recursion when the arena runs out of space
* File copying and pasting: Need to decide between linking and a dynamic array for listing the files
  * I sorta like the idea of a dynamic array, but I often run into problems with them. Maybe the arena will alleviate the ownership issues I encounter with `malloc` and I don't need to worry too much about it

## Misc Requirements
* You really gotta settle on the semantic difference between `size` and `length`
