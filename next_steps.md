# Next Steps
- [ ] Create a scratch arena
- [ ] `tree.c`- line 112: Replace `asprintf` with an arena implementation.
- [ ] Create and use an `arena_dynamic_array` function. Saves memory and avoids the complications over a linked list, and also ensures that the data is stored together in it's own memory block. Linked lists are completely out of the question.
- [ ] `tree.c`- `new_node` function: Convert `malloc` to an arena allocation
- [ ] `arena.c`: Think now it's a good time to create and implement a dynamic arena allocator into `tree_fill`
- [ ] `tree.c` - `tree_fill`: Add file name with spaces handling
- [ ] **File handling for directories:** Need to decide between linking and a dynamic array for listing the files
- [ ] Moreover, the order doesn't matter at all when creating the files, yet they will already be in order regardless upon creation. There's absolutely **no reason to use a linked list** here.
- [ ] **Final requirement:** Minimize the program's memory footprint beginning with addressing unnecessary padding caused by poorly formulated structs.

## Misc Requirements
- [ ] You really gotta settle on the semantic difference between `size` and `length`
- [ ] Look into reserving "address space" in an arena, rather than "physical memory space". This isn't necessary for the project, but worth looking into for future implementations.
