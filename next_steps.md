# Next Steps
- [ ] Now you can start comparing the source and target directories. I have no idea where to start with this, but I know you'll figure it out.
- [ ] `arena.c`: Create and implement a dynamic arena allocator to add chunks to the arena as needed.
- [ ] **Final requirement:** Minimize the program's memory footprint beginning with addressing unnecessary padding caused by poorly formulated structs.
- [x] `tree.c`- line 112: Replace `asprintf` with an arena implementation.
- [x] `tree.c`- `new_node` function: Convert `malloc` to an arena allocation
- [x] Get rid of dynamic arrays in the `tree` struct. This can just be handled with file handling.
- [x] `tree.c`: Helper function for saving `arena_temp`'s last checkpoint in the form of `subdir_path`. This way you can reset the temporary arena on each recursive call.
- [x] Remove `previous_offset` from the arena struct. It's not needed for the purposes of this project since you never resize the arena.
- [x] Make a function that lists the files in a given directory before going all in on its implementation

## Misc
- [ ] You really gotta settle on the semantic difference between `size` and `length`
- [ ] Look into reserving "address space" in an arena, rather than "physical memory space". This isn't necessary for the project, but worth looking into for future implementations.
