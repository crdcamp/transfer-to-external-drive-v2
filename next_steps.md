# Next Steps
- [ ] Come up with a plan for what to do if the directory trees are the same, or if they are different.
- [ ] Use [sendfile](https://man7.org/linux/man-pages/man2/sendfile.2.html) for copying the files to the target directory.
- [ ] Implement `tree_is_identical` into `tree_create` function to avoid double recusing over the same directory. You can therefore immediately determine if the directories are the same and make the code even more BLAZINGLY FAST.
- [ ] **Final requirement:** Minimize the program's memory footprint beginning with addressing unnecessary padding caused by poorly formulated structs.
- [x] `tree.c`- line 112: Replace `asprintf` with an arena implementation.
- [x] `tree.c`- `new_node` function: Convert `malloc` to an arena allocation
- [x] `tree.c`: Helper function for saving `arena_temp`'s last checkpoint in the form of `subdir_path`. This way you can reset the temporary arena on each recursive call.
- [x] Remove `previous_offset` from the arena struct. It's not needed for the purposes of this project since you never resize the arena.
- [x] Make a function that lists the files in a given directory before going all in on its implementation
- [x] Now you can start comparing the source and target directories. I have no idea where to start with this, but I know you'll figure it out.

## Misc
- [ ] You really gotta settle on the semantic difference between `size` and `length`, or just use the word "elements" when describing an array count.
