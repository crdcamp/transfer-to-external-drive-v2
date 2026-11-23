#!/usr/bin/env bash
# Same-name files with different contents and modification times.
# Because the target must always exactly match the source, the contents have
# to end up identical no matter which side has the newer timestamp.
source "$(dirname "$0")/lib.sh"
section "03: modification times"

begin "source newer, different content"
mkf "$SRC/f" "new content from source"
mkf "$TGT/f" "old content in target"
setage $NEW "$SRC/f"
setage $OLD "$TGT/f"
finish

begin "TARGET newer, different content"
mkf "$SRC/f" "content from source"
mkf "$TGT/f" "content in target"
setage $OLD "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "same mtime, different content"
mkf "$SRC/f" "content from source"
mkf "$TGT/f" "content in target"
setage $NEW "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "same mtime, same content"
mkf "$SRC/f" "identical"
mkf "$TGT/f" "identical"
setage $NEW "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "source newer, same content"
mkf "$SRC/f" "identical"
mkf "$TGT/f" "identical"
setage $NEW "$SRC/f"
setage $OLD "$TGT/f"
finish

begin "target newer, same content"
mkf "$SRC/f" "identical"
mkf "$TGT/f" "identical"
setage $OLD "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "same size, different content, target newer"
mkf "$SRC/f" "AAAAAAAAAA"
mkf "$TGT/f" "BBBBBBBBBB"
setage $OLD "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "source file empty, target not, source newer"
: > "$SRC/f"
mkf "$TGT/f" "not empty"
setage $NEW "$SRC/f"
setage $OLD "$TGT/f"
finish

begin "source file empty, target not, target newer"
: > "$SRC/f"
mkf "$TGT/f" "not empty"
setage $OLD "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "target file empty, source not, source older"
mkf "$SRC/f" "not empty"
: > "$TGT/f"
setage $OLD "$SRC/f"
setage $NEW "$TGT/f"
finish

begin "mixed timestamps across five files in one directory"
for n in a b c d e; do
    mkf "$SRC/$n" "source $n"
    mkf "$TGT/$n" "target $n"
done
setage $NEW "$SRC/a"; setage $OLD "$TGT/a"     # source newer
setage $OLD "$SRC/b"; setage $NEW "$TGT/b"     # target newer
setage $NEW "$SRC/c"; setage $NEW "$TGT/c"     # same time
mkf "$TGT/d" "source d"                         # same content as source
setage $NEW "$SRC/d"; setage $OLD "$TGT/d"
mkf "$TGT/e" "source e"
setage $OLD "$SRC/e"; setage $NEW "$TGT/e"
finish

begin "changed files mixed with missing and extra files"
mkf "$SRC/a" "same"
mkf "$SRC/b" "source b";  setage $NEW "$SRC/b"
mkf "$SRC/c" "only in source"
mkf "$SRC/e" "source e";  setage $OLD "$SRC/e"
mkf "$TGT/a" "same"
mkf "$TGT/b" "target b";  setage $OLD "$TGT/b"
mkf "$TGT/d" "only in target"
mkf "$TGT/e" "target e";  setage $NEW "$TGT/e"
finish

begin "large file (3 MB), source newer"
head -c 3145728 /dev/urandom > "$SRC/big"
head -c 3145728 /dev/urandom > "$TGT/big"
setage $NEW "$SRC/big"
setage $OLD "$TGT/big"
finish

begin "large file (3 MB) only in source"
head -c 3145728 /dev/urandom > "$SRC/big"
mkf "$SRC/small"
mkf "$TGT/small"
finish

# ---- metadata on copied files
begin "executable bit survives the copy"
mkf "$SRC/run.sh" "#!/bin/sh"
chmod 755 "$SRC/run.sh"
mkf "$SRC/z"
mkf "$TGT/z"
finish

begin "copied file keeps the source's mtime"
mkf "$SRC/f" "data"
setage $OLD "$SRC/f"
mkf "$SRC/z"
mkf "$TGT/z"
run_sync
if [ "$TIMED_OUT" -eq 1 ]; then
    fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
elif [ ! -f "$TGT/f" ]; then
    fail "file was not copied"
elif [ "$(file_mtime "$SRC/f")" != "$(file_mtime "$TGT/f")" ]; then
    fail "mtime differs: source=$(file_mtime "$SRC/f") target=$(file_mtime "$TGT/f")"
else
    pass
fi

begin "overwritten file keeps the source's mtime"
mkf "$SRC/f" "new data"
mkf "$TGT/f" "old data"
setage $NEW "$SRC/f"
setage $OLD "$TGT/f"
run_sync
if [ "$TIMED_OUT" -eq 1 ]; then
    fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
elif [ "$(file_mtime "$SRC/f")" != "$(file_mtime "$TGT/f")" ]; then
    fail "mtime differs: source=$(file_mtime "$SRC/f") target=$(file_mtime "$TGT/f")"
else
    pass
fi

summary
