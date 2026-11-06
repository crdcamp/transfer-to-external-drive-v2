# Notes

[Primary Source](https://www.tutorialpedia.org/blog/efficiently-traverse-directory-tree-with-opendir-readdir-and-closedir/)

Before we begin, let's define the main objective:

The original version of this project has the issue of blindly overwriting everything in the target drive. This causes the user to need to personally go into the external drive and move folders around to match the structure of the source directories if their structure ever changes.

Now, the root manner of solving this issue is to base an entire new project around the idea of copying the tree structure of the source directories.

This will involve fully encapsulating the source directory's (or directories) tree structure in a memory arena (or several memory arenas, depending on the number of tree structures we're dealing with), then mapping that directly onto the target directory (the external SSD in my case).

After (or during) the mapping of the source tree structure onto the target directory, we need to either move (more challenging) or delete files and directories that don't match the tree structure.

We also need to account for tree structures that already match the structure from the source file. **This should likely be the first check in whatever function transfers the files**.

Moving the directories could potentially be a bit more efficient (might be less efficient as well though - will need to test), while outright copying the structure and deleting all that don't match would be a bit dirtier, but a lot more easy.

Therefore, we'll begin with the deletion method to keep things initially simple

Finally, this is gonna involve a bit of recursion. Hopefully the resource can help a bit with that.

# Notes from Source
