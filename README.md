# A note to myself before dedicating more time to actually coding this project

After sinking a few hours into this project, it has resulted in a complete and unacceptable mess (at least I got the basics of an arena. There's that).

Multiple recursions over the same trees, multiple opens and reads on the same files, copying full file paths, many of which you may not need, and excessive memory usage to top it off.

Now, let's do some far more thorough research before revisiting the code. Some good starting places are [`fts`](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/fts.3.html) and [`copyfile`](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html). Start there, and **don't touch the code until you understand the data**. ["If you don't understand the data, then you don't understand the problem"](https://www.youtube.com/watch?v=rX0ItVEVjHc).

This project will be resumed after I've dedicated enough research into the best approach to this problem in a separate directory.
