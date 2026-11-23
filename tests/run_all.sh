#!/usr/bin/env bash
# Runs every test_*.sh in this directory and prints a combined summary.
#
#   SYNC_BIN=/path/to/sync ./run_all.sh
#   SYNC_BIN=/path/to/sync VERBOSE=1 ./run_all.sh      # show program output on failures
#   SYNC_BIN=/path/to/sync TIMEOUT_SECS=2 ./run_all.sh # kill hung runs sooner

dir="$(cd "$(dirname "$0")" && pwd)"
tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

total_pass=0
total_fail=0
bad_scripts=""

for t in "$dir"/test_*.sh; do
    bash "$t" 2>&1 | tee "$tmp"
    rc=${PIPESTATUS[0]}
    if [ "$rc" -eq 2 ]; then
        echo "aborting: the tests could not start (is SYNC_BIN set?)"
        exit 2
    fi
    line="$(grep -E '[0-9]+ passed, [0-9]+ failed' "$tmp" | tail -n 1)"
    p="$(printf '%s' "$line" | sed -E 's/.* ([0-9]+) passed, ([0-9]+) failed.*/\1/')"
    f="$(printf '%s' "$line" | sed -E 's/.* ([0-9]+) passed, ([0-9]+) failed.*/\2/')"
    total_pass=$((total_pass + ${p:-0}))
    total_fail=$((total_fail + ${f:-0}))
    if [ "$rc" -ne 0 ]; then bad_scripts="$bad_scripts $(basename "$t")"; fi
done

printf '\n==============================\n'
printf 'TOTAL: %d passed, %d failed\n' "$total_pass" "$total_fail"
if [ -n "$bad_scripts" ]; then
    printf 'scripts with failures:%s\n' "$bad_scripts"
    exit 1
fi
