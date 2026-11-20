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

# LEVEL 3: one level of subdirectories.
# Tests the "both directories" recursion case plus whole-directory copy/delete.
mk "$SRC" top.txt "root file" "$NEW"
mk "$TGT" top.txt "root file" "$NEW"

# Directory on both sides, contents differ -> must recurse, not copy wholesale.
mk "$SRC/docs" keep.txt    "same"       "$OLD"
mk "$TGT/docs" keep.txt    "same"       "$OLD"
mk "$SRC/docs" update.txt  "v2"         "$NEW"
mk "$TGT/docs" update.txt  "v1"         "$OLD"
mk "$SRC/docs" added.txt   "new file"   "$NEW"
mk "$TGT/docs" removed.txt "stale file" "$OLD"

# Source-only directory -> copy wholesale (directory_copy).
mk "$SRC/newdir" one.txt "1" "$NEW"
mk "$SRC/newdir" two.txt "2" "$NEW"

# Target-only directory -> recursive delete (directory_delete).
mk "$TGT/olddir" gone.txt "x" "$OLD"
mk "$TGT/olddir" gone2.txt "y" "$OLD"
# EXPECT: Target == Source. docs/ has keep, update(v2), added; removed.txt gone.
#         newdir/ exists. olddir/ no longer exists.

echo "--- BEFORE ---"; (cd "$ROOT" && find Source Target | sort)
if [[ $# -ge 1 ]]; then
  echo "--- RUNNING $1 ---"; "$1"
  echo "--- AFTER ---"; (cd "$ROOT" && find Source Target | sort)
  # diff -r checks names, types, and content (not mtimes)
  if diff -r "$SRC" "$TGT"; then echo "PASS"; else echo "FAIL"; exit 1; fi
fi

