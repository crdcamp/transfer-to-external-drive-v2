#!/usr/bin/env bash
# Edge cases: Apple junk files, unusual names, sort-order traps, symlinks,
# permissions, running twice, and argument forms.
source "$(dirname "$0")/lib.sh"
section "05: edge cases"

# Junk is deleted from BOTH sides before syncing, so the source changes too.
finish_junk() {
    ALLOW_SRC_CHANGE=1
    run_sync
    if [ "$TIMED_OUT" -eq 1 ]; then
        fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
        return
    fi
    if ! check_match; then
        fail "target does not match source" "$WORK/diff.txt"
        return
    fi
    local left=$(( $(count_junk "$SRC") + $(count_junk "$TGT") ))
    if [ "$left" -ne 0 ]; then
        fail "$left junk entries survived the purge"
        return
    fi
    pass
}

# Run twice: the second run must change nothing and the result must still match.
finish_twice() {
    run_sync
    if [ "$TIMED_OUT" -eq 1 ]; then
        fail "TIMEOUT on first run"
        return
    fi
    snapshot "$TGT" > "$WORK/run1.snap"
    run_sync
    if [ "$TIMED_OUT" -eq 1 ]; then
        fail "TIMEOUT on second run"
        return
    fi
    snapshot "$TGT" > "$WORK/run2.snap"
    if ! diff -u "$WORK/run1.snap" "$WORK/run2.snap" > "$WORK/diff.txt"; then
        fail "second run changed the target" "$WORK/diff.txt"
        return
    fi
    if ! check_match; then
        fail "target does not match source" "$WORK/diff.txt"
        return
    fi
    pass
}

# ------------------------------------------------------------ junk files
begin "junk: .DS_Store in source root"
mkf "$SRC/a"; mkf "$SRC/.DS_Store"
mkf "$TGT/a"
finish_junk

begin "junk: .DS_Store in target root"
mkf "$SRC/a"
mkf "$TGT/a"; mkf "$TGT/.DS_Store"
finish_junk

begin "junk: every kind, source and target, root level"
mkf "$SRC/a"
mkf "$SRC/.DS_Store"; mkf "$SRC/._a"
mkf "$SRC/.Trashes/501/x"; mkf "$SRC/.fseventsd/y"; mkf "$SRC/.Spotlight-V100/z"
mkf "$TGT/a"
mkf "$TGT/.DS_Store"; mkf "$TGT/._a"
mkf "$TGT/.Trashes/501/x"; mkf "$TGT/.fseventsd/y"; mkf "$TGT/.Spotlight-V100/z"
finish_junk

begin "junk: nested inside shared subdirectories"
mkf "$SRC/d/a"; mkf "$SRC/d/.DS_Store"; mkf "$SRC/d/sub/._b"
mkf "$TGT/d/a"; mkf "$TGT/d/.DS_Store"; mkf "$TGT/d/sub/._b"
finish_junk

begin "junk: only junk in source, target empty"
mkf "$SRC/.DS_Store"
finish_junk

begin "junk: source has files plus junk, target empty"
mkf "$SRC/a"; mkf "$SRC/b"; mkf "$SRC/.DS_Store"; mkf "$SRC/._a"
finish_junk

begin "junk: only junk in target, source has files"
mkf "$SRC/a"
mkf "$TGT/a"; mkf "$TGT/.DS_Store"; mkf "$TGT/._a"
finish_junk

begin "junk: junk inside a directory that only exists in source"
mkf "$SRC/d/a"; mkf "$SRC/d/.DS_Store"
mkf "$TGT/z"; mkf "$SRC/z"
finish_junk

# ------------------------------------------------------------ names
begin "hidden files that are NOT junk are kept and synced"
mkf "$SRC/.gitignore"; mkf "$SRC/.env"; mkf "$SRC/.hidden/x"; mkf "$SRC/a"
mkf "$TGT/.gitignore"; mkf "$TGT/.stale"
finish

begin "names with spaces"
mkf "$SRC/two words"; mkf "$SRC/dir with space/inner file"; mkf "$SRC/plain"
mkf "$TGT/plain"; mkf "$TGT/stale file"; mkf "$TGT/stale dir/inner"
finish

begin "names with special characters"
names=("it's" 'a&b' '$dollar' '-dash' '(paren)' 'ünïcode' '*star*' '[bracket]' 'two  spaces' 'trailing.dot.' 'semi;colon' 'hash#tag' 'pct%')
for n in "${names[@]}"; do mkf "$SRC/$n" "src $n"; done
i=0
for n in "${names[@]}"; do
    i=$((i + 1))
    if [ $((i % 2)) -eq 0 ]; then mkf "$TGT/$n" "src $n"; fi
done
mkf "$TGT/stale one"
finish

begin "directories with special characters"
mkf "$SRC/it's a dir/x"; mkf "$SRC/(dir)/x"; mkf "$SRC/dir&dir/x"
mkf "$TGT/(dir)/x"; mkf "$TGT/stale dir/x"
finish

begin "very long file name (200 chars)"
long="$(printf 'x%.0s' $(seq 1 200))"
mkf "$SRC/$long"; mkf "$SRC/a"
mkf "$TGT/a"
finish

begin "very long directory name (200 chars)"
long="$(printf 'd%.0s' $(seq 1 200))"
mkf "$SRC/$long/inner"; mkf "$SRC/a"
mkf "$TGT/a"
finish

# ------------------------------------------------------------ sort-order traps
begin "uppercase and lowercase names"
mkfiles "$SRC" Zebra apple Banana cherry Apple zebra
mkfiles "$TGT" apple cherry Yak
finish

begin "uppercase and lowercase directories"
mkdirs "$SRC" Zebra apple Banana cherry
mkdirs "$TGT" apple Yak
finish

begin "prefix names (a, a.txt, a-b, ab, a_b, 'a b', A)"
mkfiles "$SRC" a a.txt a-b ab a_b "a b" A
mkfiles "$TGT" a.txt ab "a b"
finish

begin "prefix names, target has the extras"
mkfiles "$SRC" a.txt ab "a b"
mkfiles "$TGT" a a.txt a-b ab a_b "a b" A
finish

begin "numeric-looking names (file1, file10, file2, file20, file3)"
mkfiles "$SRC" file1 file10 file2 file20 file3
mkfiles "$TGT" file10 file3 file4
finish

begin "dotted names (a, a.b, a.b.c, .a)"
mkfiles "$SRC" a a.b a.b.c .a
mkfiles "$TGT" a.b .a
finish

# ------------------------------------------------------------ symlinks
begin "symlink to a file in source, missing from target"
mkf "$SRC/real"; ln -s real "$SRC/link"
mkf "$TGT/real"
finish

begin "symlink to a directory in source, missing from target"
mkf "$SRC/realdir/x"; ln -s realdir "$SRC/linkdir"
mkf "$TGT/realdir/x"
finish

begin "broken symlink in source"
mkf "$SRC/a"; ln -s does_not_exist "$SRC/broken"
mkf "$TGT/a"
finish

begin "symlink only in target (should be deleted)"
mkf "$SRC/real"
mkf "$TGT/real"; ln -s real "$TGT/link"
finish

begin "symlink in source, regular file with same name in target"
mkf "$SRC/real"; ln -s real "$SRC/item"
mkf "$TGT/real"; mkf "$TGT/item" "regular file"
finish

begin "regular file in source, symlink with same name in target"
mkf "$SRC/real"; mkf "$SRC/item" "regular file"
mkf "$TGT/real"; ln -s real "$TGT/item"
finish

begin "symlink in source, directory with same name in target"
mkf "$SRC/real"; ln -s real "$SRC/item"
mkf "$TGT/real"; mkf "$TGT/item/x"
finish

# ------------------------------------------------------------ permissions
begin "read-only file only in target (should be deleted)"
mkf "$SRC/a"
mkf "$TGT/a"; mkf "$TGT/locked"; chmod 444 "$TGT/locked"
finish

begin "read-only file in target, source newer"
mkf "$SRC/f" "new"
mkf "$TGT/f" "old"; chmod 444 "$TGT/f"
setage $NEW "$SRC/f"; setage $OLD "$TGT/f"
finish

begin "read-only file in source gets copied with its permissions"
mkf "$SRC/ro" "locked"; chmod 444 "$SRC/ro"
mkf "$SRC/a"
mkf "$TGT/a"
finish

# ------------------------------------------------------------ run twice
begin "idempotent: mixed top-level differences"
mkfiles "$SRC" a b c d e
mkfiles "$TGT" b d f g
finish_twice

begin "idempotent: nested differences"
mkf "$SRC/d/a"; mkf "$SRC/d/new"; mkf "$SRC/e/x"
mkf "$TGT/d/a"; mkf "$TGT/d/old"; mkf "$TGT/f/y"
finish_twice

begin "idempotent: type mismatches"
mkf "$SRC/a" "file"; mkf "$SRC/b/x"
mkf "$TGT/a/x";      mkf "$TGT/b" "file"
finish_twice

begin "idempotent: changed file, source newer"
mkf "$SRC/f" "new"; mkf "$TGT/f" "old"
setage $NEW "$SRC/f"; setage $OLD "$TGT/f"
finish_twice

# ------------------------------------------------------------ arguments
begin "trailing slashes on both arguments"
mkfiles "$SRC" a b c
mkfiles "$TGT" b d
run_sync "$SRC/" "$TGT/"
if [ "$TIMED_OUT" -eq 1 ]; then
    fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
elif check_match; then
    pass
else
    fail "target does not match source" "$WORK/diff.txt"
fi

begin "trailing slash on source only"
mkfiles "$SRC" a b c
mkfiles "$TGT" b d
run_sync "$SRC/" "$TGT"
if [ "$TIMED_OUT" -eq 1 ]; then
    fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
elif check_match; then
    pass
else
    fail "target does not match source" "$WORK/diff.txt"
fi

begin "trailing slash on target only"
mkfiles "$SRC" a b c
mkfiles "$TGT" b d
run_sync "$SRC" "$TGT/"
if [ "$TIMED_OUT" -eq 1 ]; then
    fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
elif check_match; then
    pass
else
    fail "target does not match source" "$WORK/diff.txt"
fi

summary
