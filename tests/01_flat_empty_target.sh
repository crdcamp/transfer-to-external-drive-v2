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

# LEVEL 1: flat, no subdirectories, empty target.
# Exercises only the "copy source into empty target" shortcut.
mk "$SRC" a.txt "alpha"   "$NEW"
mk "$SRC" b.txt "bravo"   "$NEW"
mk "$SRC" c.txt "charlie" "$NEW"
# EXPECT: Target contains a.txt, b.txt, c.txt with identical content.

echo "--- BEFORE ---"; (cd "$ROOT" && find Source Target | sort)
if [[ $# -ge 1 ]]; then
  echo "--- RUNNING $1 ---"; "$1"
  echo "--- AFTER ---"; (cd "$ROOT" && find Source Target | sort)
  # diff -r checks names, types, and content (not mtimes)
  if diff -r "$SRC" "$TGT"; then echo "PASS"; else echo "FAIL"; exit 1; fi
fi

