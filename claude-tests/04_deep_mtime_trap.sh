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

# LEVEL 4: deep nesting + the directory-mtime trap.
# Build identical trees, pin EVERY directory mtime to the same value, THEN edit
# files in place. In-place edits never bump ancestor directory mtimes, so a
# "source dir newer -> recurse/copy" gate would skip every change below.
DEEP=l1/l2/l3/l4/l5
for side in "$SRC" "$TGT"; do
  mk "$side"          root.txt   "root"    "$OLD"
  mk "$side/l1"       a.txt      "a"       "$OLD"
  mk "$side/l1/l2"    b.txt      "b"       "$OLD"
  mk "$side/l1/l2/l3" c.txt      "c"       "$OLD"
  mk "$side/$DEEP"   deep.txt   "deep v1" "$OLD"
  mk "$side/l1/sibling" s.txt    "s"       "$OLD"
done
# Pin all directory mtimes to the same value on both sides
find "$SRC" "$TGT" -type d -exec touch -t "$OLD" {} +

# --- in-place edits on the SOURCE only (dir mtimes untouched) ---
printf "deep v2\n" > "$SRC/$DEEP/deep.txt";    touch -t "$NEW" "$SRC/$DEEP/deep.txt"
printf "b v2\n"    > "$SRC/l1/l2/b.txt";        touch -t "$NEW" "$SRC/l1/l2/b.txt"

# --- structural changes deep in the tree ---
mk "$SRC/l1/l2/l3" added_deep.txt "new deep file" "$NEW"     # source-only, deep
mk "$TGT/l1/sibling/extra" junk.txt "stale"        "$OLD"     # target-only dir, deep
# Re-pin ancestors we just touched so the trap stays armed
touch -t "$OLD" "$SRC/l1/l2/l3" "$TGT/l1/sibling"
# EXPECT: Target == Source. deep.txt = "deep v2", b.txt = "b v2",
#         added_deep.txt present, l1/sibling/extra/ removed.

echo "--- BEFORE ---"; (cd "$ROOT" && find Source Target | sort)
if [[ $# -ge 1 ]]; then
  echo "--- RUNNING $1 ---"; "$1"
  echo "--- AFTER ---"; (cd "$ROOT" && find Source Target | sort)
  # diff -r checks names, types, and content (not mtimes)
  if diff -r "$SRC" "$TGT"; then echo "PASS"; else echo "FAIL"; exit 1; fi
fi

