#!/usr/bin/env bash
# Run from the project root (the directory that contains TestDirs/).
# Usage: ./THIS_SCRIPT.sh [path/to/transfer_binary]
#   No argument: just builds TestDirs/Source and TestDirs/Target.
#   With binary: builds, runs it, then checks `diff -r Source Target`.
set -euo pipefail
ROOT="TestDirs"; SRC="$ROOT/Source"; TGT="$ROOT/Target"
OLD=202001010000.00   # stale timestamp
NEW=202401010000.00   # fresh timestamp
rm -rf "$ROOT"; mkdir -p "$SRC" "$TGT"
# mk <dir> <name> <content> <mtime>
mk() { mkdir -p "$1"; printf "%s\n" "$3" > "$1/$2"; touch -t "$4" "$1/$2"; }

# LEVEL 5: edge cases.
# (a) type mismatches, (b) Apple junk, (c) empty dirs, (d) awkward names/sort order.

# (a) Same name, different type
mk "$SRC"         was_dir_now_file "I am a file now" "$NEW"     # source file...
mk "$TGT/was_dir_now_file" inner.txt "inside"        "$OLD"     # ...target directory
mk "$SRC/was_file_now_dir" inner.txt "inside"        "$NEW"     # source directory...
mk "$TGT"         was_file_now_dir "I was a file"    "$OLD"     # ...target file

# (b) Apple junk. purge_junk removes it from BOTH sides, so the diff stays clean.
mk "$SRC"          .DS_Store "junk" "$NEW"
mk "$TGT"          .DS_Store "junk" "$OLD"
mk "$SRC"          ._resource_fork "junk" "$NEW"
mk "$SRC/sub"      .DS_Store "junk" "$NEW"
mk "$SRC/sub"      real.txt  "real" "$NEW"
mkdir -p "$TGT/.Trashes"; printf "junk\n" > "$TGT/.Trashes/x"
mkdir -p "$SRC/.fseventsd"; printf "junk\n" > "$SRC/.fseventsd/y"

# (c) Empty directories
mkdir -p "$SRC/empty_src_only"
mkdir -p "$SRC/empty_both" "$TGT/empty_both"
mkdir -p "$TGT/empty_tgt_only"
mk "$SRC/src_has_files_tgt_empty" f.txt "x" "$NEW"
mkdir -p "$TGT/src_has_files_tgt_empty"

# (d) Awkward names + sort-order traps (strcmp is bytewise: uppercase < lowercase,
#     '-' (0x2D) < '.' (0x2E) < letters, prefixes sort before longer names)
mk "$SRC" "with space.txt" "space" "$NEW"
mk "$SRC" "Zebra.txt"      "Z"     "$NEW"
mk "$SRC" "apple.txt"      "a"     "$NEW"
mk "$SRC" "a-b.txt"        "ab"    "$NEW"
mk "$SRC" "a"              "a"     "$NEW"
mk "$TGT" "apple.txt"      "a"     "$NEW"
mk "$TGT" "a.txt"          "stale" "$OLD"

# EXPECT: Target == Source (minus junk, which is gone from both):
#   was_dir_now_file is a FILE, was_file_now_dir is a DIRECTORY,
#   all empty dirs mirrored except empty_tgt_only (deleted),
#   src_has_files_tgt_empty/f.txt copied, a.txt deleted, sort-trap names all present.

echo "--- BEFORE ---"; (cd "$ROOT" && find Source Target | sort)
if [[ $# -ge 1 ]]; then
  echo "--- RUNNING $1 ---"; "$1"
  echo "--- AFTER ---"; (cd "$ROOT" && find Source Target | sort)
  # diff -r checks names, types, and content (not mtimes)
  if diff -r "$SRC" "$TGT"; then echo "PASS"; else echo "FAIL"; exit 1; fi
fi

