# Where You Left Off

`tree.c`- line 112: Replace `asprintf` with an arena implementation.

## Next Steps
* `tree.c`- `new_node` function: Convert function from `malloc` to an arena allocation
* `tree.c`- `tree_fill` function: Implement file paths into the tree structure
  * Once that's done- try to make it it's own function
* `tree.c`- `tree_fill` function: Add arena expansion during recursion when the arena runs out of space

## Misc Requirements
* You really gotta settle on the semantic difference between `size` and `length`
