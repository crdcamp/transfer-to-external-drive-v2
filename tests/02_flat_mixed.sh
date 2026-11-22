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

# LEVEL 2: flat, no subdirectories, every per-file case at one depth.
# Names are chosen so source-only/target-only entries sit at the START,
# MIDDLE, and END of the sorted lists (exercises a merge walk and the
# "one list runs out first" crash).
mk "$SRC" b_same.txt     "identical"      "$OLD"
mk "$TGT" b_same.txt     "identical"      "$OLD"

mk "$SRC" c_modified.txt "new content"    "$NEW"
mk "$TGT" c_modified.txt "old content"    "$OLD"

# Source is OLDER than target but content differs (e.g. restored backup).
# A '>' mtime test skips this; a '!=' test copies it.
mk "$SRC" d_older.txt    "restored"       "$OLD"
mk "$TGT" d_older.txt    "newer-but-stale" "$NEW"

mk "$SRC" a_src_only.txt "only in source" "$NEW"       # sorts first
mk "$SRC" m_src_only.txt "only in source" "$NEW"       # sorts middle
mk "$SRC" y_src_only.txt "only in source" "$NEW"       # sorts last (target runs out)

mk "$TGT" 0_tgt_only.txt "only in target" "$OLD"       # sorts before everything
mk "$TGT" e_tgt_only.txt "only in target" "$OLD"       # sorts middle
mk "$TGT" z_tgt_only.txt "only in target" "$OLD"       # sorts after everything (source runs out)
# EXPECT: Target == Source exactly. 0_/e_/z_ target-only files deleted;
#         a_/m_/y_ copied; c_ and d_ overwritten; b_ untouched.

echo "--- BEFORE ---"; (cd "$ROOT" && find Source Target | sort)
if [[ $# -ge 1 ]]; then
  echo "--- RUNNING $1 ---"; "$1"
  echo "--- AFTER ---"; (cd "$ROOT" && find Source Target | sort)
  # diff -r checks names, types, and content (not mtimes)
  if diff -r "$SRC" "$TGT"; then echo "PASS"; else echo "FAIL"; exit 1; fi
fi

