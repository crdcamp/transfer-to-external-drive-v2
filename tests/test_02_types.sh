#!/usr/bin/env bash
# File vs. directory cases at the top level of the directories.
# The same name can be a file on one side and a directory on the other,
# and directories get copied / deleted recursively.
source "$(dirname "$0")/lib.sh"
section "02: file vs. directory"

# ---- type mismatch on the same name
begin "source file, target directory (same name)"
mkf "$SRC/item" "i am a file"
mkf "$TGT/item/inside.txt" "i am inside a directory"
finish

begin "source directory, target file (same name)"
mkf "$SRC/item/inside.txt" "i am inside a directory"
mkf "$TGT/item" "i am a file"
finish

begin "source file, EMPTY target directory (same name)"
mkf "$SRC/item" "i am a file"
mkdir -p "$TGT/item"
finish

begin "EMPTY source directory, target file (same name)"
mkdir -p "$SRC/item"
mkf "$TGT/item" "i am a file"
finish

begin "mismatch (file vs dir) at the FIRST position"
mkfiles "$SRC" b c
mkf "$SRC/a" "file"
mkfiles "$TGT" b c
mkf "$TGT/a/x.txt"
finish

begin "mismatch (dir vs file) at the FIRST position"
mkfiles "$SRC" b c
mkf "$SRC/a/x.txt"
mkfiles "$TGT" b c
mkf "$TGT/a" "file"
finish

begin "mismatch (file vs dir) in the MIDDLE"
mkfiles "$SRC" a c
mkf "$SRC/b" "file"
mkfiles "$TGT" a c
mkf "$TGT/b/x.txt"
finish

begin "mismatch (dir vs file) in the MIDDLE"
mkfiles "$SRC" a c
mkf "$SRC/b/x.txt"
mkfiles "$TGT" a c
mkf "$TGT/b" "file"
finish

begin "mismatch (file vs dir) at the LAST position"
mkfiles "$SRC" a b
mkf "$SRC/z" "file"
mkfiles "$TGT" a b
mkf "$TGT/z/x.txt"
finish

begin "mismatch (dir vs file) at the LAST position"
mkfiles "$SRC" a b
mkf "$SRC/z/x.txt"
mkfiles "$TGT" a b
mkf "$TGT/z" "file"
finish

begin "back-to-back mismatches in both directions"
mkf "$SRC/a" "file";   mkf "$TGT/a/x.txt"
mkf "$SRC/b/x.txt";    mkf "$TGT/b" "file"
mkf "$SRC/c" "file";   mkf "$TGT/c/x.txt"
mkf "$SRC/d/x.txt";    mkf "$TGT/d" "file"
finish

begin "mismatch plus extra file on each side"
mkf "$SRC/a" "file"
mkf "$SRC/b"
mkf "$TGT/a/x.txt"
mkf "$TGT/c"
finish

begin "type mismatch where the directory holds a deep tree"
mkf "$SRC/item" "file"
mkf "$TGT/item/l1/l2/l3/leaf.txt"
mkf "$TGT/item/l1/other.txt"
mkf "$TGT/item/top.txt"
finish

# ---- directories only
begin "dirs only: identical, all empty"
mkdirs "$SRC" a b c
mkdirs "$TGT" a b c
finish

begin "dirs only: identical, with content"
mkf "$SRC/a/x"; mkf "$SRC/b/y"; mkf "$SRC/c/z"
mkf "$TGT/a/x"; mkf "$TGT/b/y"; mkf "$TGT/c/z"
finish

begin "dirs only: last entries differ (a b d vs a b c)"
mkdirs "$SRC" a b d
mkdirs "$TGT" a b c
finish

begin "dirs only: no overlap, interleaved (a c vs b d)"
mkdirs "$SRC" a c
mkdirs "$TGT" b d
finish

begin "dirs only: extra directories at END of target"
mkdirs "$SRC" a b
mkdirs "$TGT" a b c d
finish

begin "dirs only: extra directories at END of source"
mkdirs "$SRC" a b c d
mkdirs "$TGT" a b
finish

begin "dirs only: extra directories at START of target"
mkdirs "$SRC" c d
mkdirs "$TGT" a b c d
finish

begin "dirs only: extra directories at START of source"
mkdirs "$SRC" a b c d
mkdirs "$TGT" c d
finish

begin "dirs only: extra directory in MIDDLE of source"
mkdirs "$SRC" a b c
mkdirs "$TGT" a c
finish

begin "dirs only: extra directory in MIDDLE of target"
mkdirs "$SRC" a c
mkdirs "$TGT" a b c
finish

begin "dirs only: no overlap, source sorts first"
mkdirs "$SRC" a b c
mkdirs "$TGT" x y z
finish

begin "dirs only: no overlap, source sorts last"
mkdirs "$SRC" x y z
mkdirs "$TGT" a b c
finish

begin "dirs only: target empty"
mkf "$SRC/a/x"; mkf "$SRC/b/y"
finish

begin "dirs only: source empty"
mkf "$TGT/a/x"; mkf "$TGT/b/y"
finish

# ---- mixed lists
begin "mixed: extra directory in source between matching files"
mkfiles "$SRC" a c
mkf "$SRC/b/x.txt"
mkfiles "$TGT" a c
finish

begin "mixed: extra directory in target between matching files"
mkfiles "$SRC" a c
mkfiles "$TGT" a c
mkf "$TGT/b/x.txt"
finish

begin "mixed: extra file in source between matching directories"
mkdirs "$SRC" a c
mkf "$SRC/b"
mkdirs "$TGT" a c
finish

begin "mixed: extra file in target between matching directories"
mkdirs "$SRC" a c
mkdirs "$TGT" a c
mkf "$TGT/b"
finish

begin "mixed: directories and files, nothing in common"
mkf "$SRC/a/x"; mkf "$SRC/c"; mkf "$SRC/e/x"
mkf "$TGT/b"; mkf "$TGT/d/x"; mkf "$TGT/f"
finish

# ---- recursive copy / delete of whole trees
begin "copy a deep tree that is missing from target"
mkf "$SRC/keep"
mkf "$SRC/tree/l1/l2/l3/leaf.txt"
mkf "$SRC/tree/l1/l2/other.txt"
mkf "$SRC/tree/l1/side.txt"
mkdir -p "$SRC/tree/l1/emptydir"
mkf "$SRC/tree/top.txt"
mkf "$TGT/keep"
finish

begin "delete a deep tree that is missing from source"
mkf "$SRC/keep"
mkf "$TGT/keep"
mkf "$TGT/tree/l1/l2/l3/leaf.txt"
mkf "$TGT/tree/l1/l2/other.txt"
mkf "$TGT/tree/l1/side.txt"
mkdir -p "$TGT/tree/l1/emptydir"
mkf "$TGT/tree/top.txt"
finish

begin "copy several trees into an empty target"
for d in a b c; do
    mkf "$SRC/$d/l1/l2/leaf.txt"
    mkf "$SRC/$d/top.txt"
done
finish

begin "delete several trees when source is empty"
for d in a b c; do
    mkf "$TGT/$d/l1/l2/leaf.txt"
    mkf "$TGT/$d/top.txt"
done
finish

summary
