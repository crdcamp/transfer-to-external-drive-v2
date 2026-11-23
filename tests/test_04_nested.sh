#!/usr/bin/env bash
# Nested cases. Whenever a directory exists on both sides, the sync has to
# descend into it, so these cases exercise the recursion.
source "$(dirname "$0")/lib.sh"
section "04: nested directories (recursion)"

begin "extra file inside a shared subdirectory (target)"
mkf "$SRC/d/a"
mkf "$TGT/d/a"
mkf "$TGT/d/extra"
finish

begin "missing file inside a shared subdirectory (source has it)"
mkf "$SRC/d/a"
mkf "$SRC/d/b"
mkf "$TGT/d/a"
finish

begin "changed file inside a shared subdirectory (source newer)"
mkf "$SRC/d/f" "new"
mkf "$TGT/d/f" "old"
setage $NEW "$SRC/d/f"
setage $OLD "$TGT/d/f"
finish

begin "changed file inside a shared subdirectory (target newer)"
mkf "$SRC/d/f" "source"
mkf "$TGT/d/f" "target"
setage $OLD "$SRC/d/f"
setage $NEW "$TGT/d/f"
finish

begin "extra subdirectory inside a shared subdirectory (target)"
mkf "$SRC/d/a"
mkf "$TGT/d/a"
mkf "$TGT/d/sub/x"
finish

begin "missing subdirectory inside a shared subdirectory (source has it)"
mkf "$SRC/d/a"
mkf "$SRC/d/sub/x"
mkf "$TGT/d/a"
finish

begin "file vs directory inside a shared subdirectory"
mkf "$SRC/d/item" "file"
mkf "$SRC/d/z"
mkf "$TGT/d/item/inside"
mkf "$TGT/d/z"
finish

begin "directory vs file inside a shared subdirectory"
mkf "$SRC/d/item/inside"
mkf "$SRC/d/z"
mkf "$TGT/d/item" "file"
mkf "$TGT/d/z"
finish

begin "source subdirectory empty, target subdirectory has files"
mkdir -p "$SRC/d"
mkf "$SRC/keep"
mkf "$TGT/keep"
mkf "$TGT/d/a"
mkf "$TGT/d/b"
finish

begin "target subdirectory empty, source subdirectory has files"
mkf "$SRC/d/a"
mkf "$SRC/d/b"
mkf "$SRC/keep"
mkdir -p "$TGT/d"
mkf "$TGT/keep"
finish

begin "empty directory only in source (nested)"
mkf "$SRC/d/a"
mkdir -p "$SRC/d/empty"
mkf "$TGT/d/a"
finish

begin "empty directory only in target (nested)"
mkf "$SRC/d/a"
mkf "$TGT/d/a"
mkdir -p "$TGT/d/empty"
finish

begin "5 levels deep, only the leaf differs"
mkf "$SRC/l1/l2/l3/l4/l5/leaf" "new"
mkf "$TGT/l1/l2/l3/l4/l5/leaf" "old"
mkf "$SRC/l1/l2/l3/l4/l5/same"
mkf "$TGT/l1/l2/l3/l4/l5/same"
setage $NEW "$SRC/l1/l2/l3/l4/l5/leaf"
setage $OLD "$TGT/l1/l2/l3/l4/l5/leaf"
finish

begin "5 levels deep, target has extra branch near the top"
mkf "$SRC/l1/l2/l3/l4/l5/leaf"
mkf "$TGT/l1/l2/l3/l4/l5/leaf"
mkf "$TGT/l1/extra/deep/file"
finish

begin "5 levels deep, source has extra branch near the top"
mkf "$SRC/l1/l2/l3/l4/l5/leaf"
mkf "$SRC/l1/extra/deep/file"
mkf "$TGT/l1/l2/l3/l4/l5/leaf"
finish

begin "same file name in different directories (no cross-matching)"
mkf "$SRC/a/x" "source a/x"
mkf "$SRC/c/x" "source c/x"
mkf "$TGT/b/x" "target b/x"
mkf "$TGT/c/x" "target c/x"
setage $NEW "$SRC/c/x"
setage $OLD "$TGT/c/x"
finish

begin "differences in the first directory AND in later siblings"
for d in a b c d; do
    mkf "$SRC/$d/same"
    mkf "$TGT/$d/same"
    mkf "$SRC/$d/src_only_$d"
    mkf "$TGT/$d/tgt_only_$d"
done
finish

begin "recursion returns correctly: shared dir followed by top-level differences"
mkf "$SRC/a/x"; mkf "$TGT/a/x"; mkf "$TGT/a/extra"
mkf "$SRC/b"                      # only in source
mkf "$TGT/c"                      # only in target
mkf "$SRC/d/x"; mkf "$TGT/d/x"; mkf "$SRC/d/new"
finish

begin "mix of everything at several levels"
# top level
mkf "$SRC/a_same";            mkf "$TGT/a_same"
mkf "$SRC/b_src_only"
mkf "$TGT/c_tgt_only"
mkf "$SRC/d_changed" "new";   mkf "$TGT/d_changed" "old"
setage $NEW "$SRC/d_changed"; setage $OLD "$TGT/d_changed"
mkf "$SRC/e_file_vs_dir" "f"; mkf "$TGT/e_file_vs_dir/x"
mkf "$SRC/f_dir_vs_file/x";   mkf "$TGT/f_dir_vs_file" "f"
# shared directory, with its own differences
mkf "$SRC/g_dir/same";        mkf "$TGT/g_dir/same"
mkf "$SRC/g_dir/src_only"
mkf "$TGT/g_dir/tgt_only"
mkf "$SRC/g_dir/sub/leaf" "new"; mkf "$TGT/g_dir/sub/leaf" "old"
setage $NEW "$SRC/g_dir/sub/leaf"; setage $OLD "$TGT/g_dir/sub/leaf"
mkf "$TGT/g_dir/old_tree/x/y"
mkf "$SRC/g_dir/new_tree/x/y"
# whole trees on one side
mkf "$SRC/h_src_tree/x/y"
mkf "$TGT/i_tgt_tree/x/y"
finish

begin "wide tree: 20 dirs x 10 files with scattered differences"
for d in $(seq -w 1 20); do
    for f in $(seq -w 1 10); do
        mkf "$SRC/dir$d/file$f" "dir$d file$f"
        mkf "$TGT/dir$d/file$f" "dir$d file$f"
    done
done
for d in 03 07 11; do rm "$TGT/dir$d/file05"; done          # missing from target
for d in 02 09 15; do mkf "$TGT/dir$d/extra"; done          # extra in target
rm -rf "$TGT/dir05"                                         # whole dir missing
mkf "$SRC/dir12/file02" "changed in source"
setage $NEW "$SRC/dir12/file02"
setage $OLD "$TGT/dir12/file02"
mkf "$SRC/dir20/newfile"
mkf "$TGT/dir21/orphan"
finish

begin "deep chain: 15 levels, identical"
p="l1/l2/l3/l4/l5/l6/l7/l8/l9/l10/l11/l12/l13/l14/l15"
mkf "$SRC/$p/leaf"
mkf "$TGT/$p/leaf"
finish

begin "deep chain: 15 levels, leaf missing in target"
p="l1/l2/l3/l4/l5/l6/l7/l8/l9/l10/l11/l12/l13/l14/l15"
mkf "$SRC/$p/leaf"
mkdir -p "$TGT/$p"
finish

summary
