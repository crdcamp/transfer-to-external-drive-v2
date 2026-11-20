# Overview

**This project is a work in progress.**

One thing this project taught me: ["If you don't understand the data, you don't understand the problem"](https://www.youtube.com/watch?v=rX0ItVEVjHc).

This is my first serious systems programming project designed to "sync" a source directory with multiple target directories so that they perfectly mirror each other. It is meant to vastly improve its [previous version](https://github.com/crdcamp/transfer-to-external-drive) and involves recursion, a small memory arena implementation, and some advanced file management. **This project currently only works on MacOS**, but will eventually be cross-compatible with Linux.

While the previous version simply used system calls to the `cp` command, this one gets a lot more technical and (hopefully) a **lot** faster.

This project has also slowly turned into a basis for turning my extra laptop into a server that syncs with directories of my choosing. Might even make a cron job to automate that at some point...

This `README.md` will be finalized when the project has reached completion.
