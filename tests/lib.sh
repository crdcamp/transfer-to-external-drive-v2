#!/usr/bin/env bash
# Shared helpers for the sync tests. Sourced by the test_*.sh scripts; not run directly.
#
# Environment variables:
#   SYNC_BIN      path to your compiled sync binary          (default: ./sync)
#   TIMEOUT_SECS  kill a run after this many seconds         (default: 5)
#   VERBOSE       1 = show the program's output on failures  (default: 0)
#
# The binary is invoked as:   $SYNC_BIN <source_dir> <target_dir>
# If your CLI differs, edit run_sync() below. Nothing else needs to change.

SYNC_BIN="${SYNC_BIN:-./sync}"
TIMEOUT_SECS="${TIMEOUT_SECS:-5}"
VERBOSE="${VERBOSE:-0}"

if [ ! -x "$SYNC_BIN" ] && ! command -v "$SYNC_BIN" >/dev/null 2>&1; then
    echo "error: '$SYNC_BIN' is not executable. Set SYNC_BIN=/path/to/your/binary" >&2
    exit 2
fi

# Everything happens inside a throwaway directory
WORK="$(mktemp -d "${TMPDIR:-/tmp}/synctest.XXXXXX")" || exit 1
SRC="$WORK/source"
TGT="$WORK/target"
LOG="$WORK/sync.log"

PASS=0
FAIL=0
CURRENT=""
RC=0
TIMED_OUT=0
ALLOW_SRC_CHANGE=0

OLD=202001010000    # timestamps for setage()
NEW=202401010000

cleanup() {
    chmod -R u+rwX "$WORK" 2>/dev/null
    rm -rf "${WORK:?}"
}
trap cleanup EXIT

# ---------------------------------------------------------------- builders

# mkf <path> [content]   create a file (and parent dirs)
mkf() {
    mkdir -p "$(dirname "$1")"
    printf '%s\n' "${2-content of $(basename "$1")}" > "$1"
}

# mkfiles <dir> <name>...   create several files directly inside <dir>
mkfiles() {
    local d=$1 n
    shift
    for n in "$@"; do mkf "$d/$n"; done
}

# mkdirs <dir> <name>...    create several (empty) directories inside <dir>
mkdirs() {
    local d=$1 n
    shift
    for n in "$@"; do mkdir -p "$d/$n"; done
}

# setage <timestamp> <path>   set mtime, e.g. setage $OLD file
setage() { touch -t "$1" "$2"; }

file_mtime() {
    if [ "$(uname)" = "Darwin" ]; then stat -f %m "$1"; else stat -c %Y "$1"; fi
}

count_junk() {
    find "$1" \( -name '.DS_Store' -o -name '.Spotlight-V100' -o -name '.fseventsd' \
                 -o -name '.Trashes' -o -name '._*' \) | wc -l | tr -d ' '
}

# ---------------------------------------------------------------- running

run_with_timeout() {
    local secs=$1 pid watcher rc
    shift
    "$@" &
    pid=$!
    ( sleep "$secs"; kill -9 "$pid" 2>/dev/null ) &
    watcher=$!
    wait "$pid" 2>/dev/null
    rc=$?
    kill "$watcher" 2>/dev/null
    wait "$watcher" 2>/dev/null
    return $rc
}

# run_sync [source] [target]   sets RC and TIMED_OUT
run_sync() {
    local s="${1-$SRC}" t="${2-$TGT}"
    TIMED_OUT=0
    run_with_timeout "$TIMEOUT_SECS" "$SYNC_BIN" "$s" "$t" > "$LOG" 2>&1
    RC=$?
    if [ "$RC" -eq 137 ]; then TIMED_OUT=1; fi
}

# ---------------------------------------------------------------- comparing

# One line per entry: type, permissions, path, and a checksum for files.
snapshot() {
    ( cd "$1" || exit 1
      find . -mindepth 1 | LC_ALL=C sort | while IFS= read -r p; do
          mode="$(ls -ld "$p" | cut -c1-10)"
          if [ -L "$p" ]; then
              printf 'l %s -> %s\n' "$p" "$(readlink "$p")"
          elif [ -d "$p" ]; then
              printf 'd %s %s\n' "$mode" "$p"
          elif [ -f "$p" ]; then
              printf 'f %s %s %s\n' "$mode" "$p" "$(cksum < "$p")"
          else
              printf '? %s %s\n' "$mode" "$p"
          fi
      done )
}

# 0 if target exactly matches source, otherwise 1 (diff goes to $WORK/diff.txt)
check_match() {
    snapshot "$SRC" > "$WORK/src.snap"
    snapshot "$TGT" > "$WORK/tgt.snap"
    diff -u "$WORK/src.snap" "$WORK/tgt.snap" > "$WORK/diff.txt"
}

# ---------------------------------------------------------------- reporting

section() { printf '\n== %s ==\n' "$1"; }

pass() {
    PASS=$((PASS + 1))
    printf '  PASS  %s\n' "$CURRENT"
}

# fail <message> [difffile]
fail() {
    FAIL=$((FAIL + 1))
    printf '  FAIL  %s\n        %s\n' "$CURRENT" "$1"
    if [ -n "${2-}" ] && [ -s "$2" ]; then
        tail -n +3 "$2" | sed 's/^/          /'
    fi
    if [ "$VERBOSE" = "1" ] && [ -s "$LOG" ]; then
        echo "        --- program output (last 15 lines) ---"
        tail -n 15 "$LOG" | sed 's/^/          /'
    fi
}

# begin <test name>   fresh, empty source and target directories
begin() {
    CURRENT="$1"
    ALLOW_SRC_CHANGE=0
    chmod -R u+rwX "$SRC" "$TGT" 2>/dev/null
    rm -rf "${SRC:?}" "${TGT:?}"
    mkdir -p "$SRC" "$TGT"
}

# finish   run the sync, then require:
#   1. it didn't hang
#   2. target exactly matches source (names, types, permissions, contents)
#   3. source wasn't modified (unless ALLOW_SRC_CHANGE=1)
#   4. exit status 0
finish() {
    snapshot "$SRC" > "$WORK/before.snap"
    run_sync

    if [ "$TIMED_OUT" -eq 1 ]; then
        fail "TIMEOUT after ${TIMEOUT_SECS}s (infinite loop?)"
        return
    fi
    if ! check_match; then
        fail "target does not match source  ('-' = only in source, '+' = only in target)" "$WORK/diff.txt"
        return
    fi
    if [ "$ALLOW_SRC_CHANGE" -ne 1 ]; then
        snapshot "$SRC" > "$WORK/after.snap"
        if ! diff -u "$WORK/before.snap" "$WORK/after.snap" > "$WORK/diff.txt"; then
            fail "source directory was modified" "$WORK/diff.txt"
            return
        fi
    fi
    if [ "$RC" -ne 0 ]; then
        fail "directories match, but the program exited with status $RC"
        return
    fi
    pass
}

summary() {
    printf '\n%s: %d passed, %d failed\n' "$(basename "$0")" "$PASS" "$FAIL"
    [ "$FAIL" -eq 0 ]
}
