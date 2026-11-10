# Next Steps
- [ ] Add `char **file_paths` to the `Node` struct and use `arena_string_concat` to add file paths to the struct (this is an initial idea and could be a completely incorrect implementation for this). **Do some extensive planning before committing to this approach**.
- [ ] `arena.c`: Think now it's a good time to create and implement a dynamic arena allocator into `tree_fill`. However, I think the arena extension code should definitely be outside of `arena_allocate`, since sometimes you're not gonna want to extend the arena's memory.
- [ ] `tree.c` - `tree_fill`: Add file name with spaces handling
- [ ] **File handling for directories:** Need to decide between linking and a dynamic array for listing the files
- [ ] Moreover, the order doesn't matter at all when creating the files, yet they will already be in order regardless upon creation. There's absolutely **no reason to use a linked list** here.
- [ ] **Final requirement:** Minimize the program's memory footprint beginning with addressing unnecessary padding caused by poorly formulated structs.
- [x] `tree.c`- line 112: Replace `asprintf` with an arena implementation.
- [x] `tree.c`- `new_node` function: Convert `malloc` to an arena allocation

## Misc
- [ ] You really gotta settle on the semantic difference between `size` and `length`
- [ ] Look into reserving "address space" in an arena, rather than "physical memory space". This isn't necessary for the project, but worth looking into for future implementations.
