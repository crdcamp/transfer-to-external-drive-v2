# Introduction

**This project is a work in progress.**

This is my first serious systems programming project with a strong focus on performance and optimization. The code implements a [memory arena](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator), tree structures, recursion, and file management. It's an *aggressive* improvement on my previous project, [transfer-to-external-drive](https://github.com/crdcamp/transfer-to-external-drive).

## A note to myself before dedicating more time to actually coding this project

After sinking a few hours into this project, it has resulted in a complete and unacceptable mess (congrats on figuring out how to use a memory arena though. I'm very happy you at least got the basics on that).

Multiple recursions over the same trees, multiple opens and reads on the same files, copying full file paths, many of which you may not need, and excessive memory usage to top it off.

Now, let's do some far more thorough research before revisiting the code. Some good starting places are [`fts`](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/fts.3.html) and [`copyfile`](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html). Start there, and **don't touch the code until you understand the data**. ["If you don't understand the data, then you don't understand the problem"](https://www.youtube.com/watch?v=rX0ItVEVjHc).

This `README.md` will now be dedicated to thorough research and notes regarding file management specifically for MacOS.

## The Goal

The intends to accomplish a seemingly simple task: intake source directories and copy their contents to target directories. In the [previous version](https://github.com/crdcamp/transfer-to-external-drive), I used system calls to the `cp` command to accomplish this. However, this doesn't preserve directory/file structures that already exist, resulting in duplicated data being copied to the target directory - a waste of memory, system resources, and time. I felt that with a more hands-on approach this functionality could be greatly improved and *much* faster.

So, this second iteration (more of a complete overhaul) is far more robust. The finished project will store the source directory's file structure in a "metadata tree structure" stored in a memory arena while the program runs. This tree structure will then be used to ensure that the directory structures between the source and target directory perfectly match. As a result, the original file structure is essentially "synced" between both directories.

I use the previous version of this project to automate the transfer of certain directories to an external drive. Since I like things to be well organized, the structure of the source directories I chose changes pretty often. As a result, I need to occasionally wipe the external drive and copy everything fresh to preserve memory and the file structure. Not only does this partially defeat the purpose of the first version, it also gives a great excuse for a fun and much more interesting new project in the form of this one. I've also been looking for an excuse to learn memory arenas as well.

Until this code is complete, goodbye!
