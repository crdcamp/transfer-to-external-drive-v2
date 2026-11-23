#!/usr/bin/env bash
# Name-ordering cases. Files only, so type handling isn't mixed in.
# Covers every path through: source name < / > / == target name,
# and either list running out before the other.
source "$(dirname "$0")/lib.sh"
section "01: name ordering"

# ---- everything matches
begin "identical lists"
mkfiles "$SRC" a b c
mkfiles "$TGT" a b c
finish

begin "single file, same name"
mkfiles "$SRC" a
mkfiles "$TGT" a
finish

# ---- source has something target lacks (copy)
begin "extra file at START of source"
mkfiles "$SRC" a b c
mkfiles "$TGT" b c
finish

begin "extra file in MIDDLE of source"
mkfiles "$SRC" a b c
mkfiles "$TGT" a c
finish

begin "extra file at END of source"
mkfiles "$SRC" a b c
mkfiles "$TGT" a b
finish

begin "several extra files in source (start, middle, end)"
mkfiles "$SRC" a b c d e f g
mkfiles "$TGT" b d f
finish

# ---- target has something source lacks (delete)
begin "extra file at START of target"
mkfiles "$SRC" b c
mkfiles "$TGT" a b c
finish

begin "extra file in MIDDLE of target"
mkfiles "$SRC" a c
mkfiles "$TGT" a b c
finish

begin "extra file at END of target"
mkfiles "$SRC" a b
mkfiles "$TGT" a b c
finish

begin "several extra files in target (start, middle, end)"
mkfiles "$SRC" b d f
mkfiles "$TGT" a b c d e f g
finish

# ---- both differ at once
begin "last entries differ (both lists end on different names)"
mkfiles "$SRC" a b d
mkfiles "$TGT" a b c
finish

begin "first entries differ, rest match"
mkfiles "$SRC" a c d
mkfiles "$TGT" b c d
finish

begin "single file each, different names (source sorts first)"
mkfiles "$SRC" a
mkfiles "$TGT" b
finish

begin "single file each, different names (source sorts last)"
mkfiles "$SRC" b
mkfiles "$TGT" a
finish

begin "no overlap, every source name sorts before every target name"
mkfiles "$SRC" a b c
mkfiles "$TGT" x y z
finish

begin "no overlap, every source name sorts after every target name"
mkfiles "$SRC" x y z
mkfiles "$TGT" a b c
finish

begin "no overlap, names interleaved"
mkfiles "$SRC" a c e g
mkfiles "$TGT" b d f h
finish

begin "source is a one-file subset of target"
mkfiles "$SRC" b
mkfiles "$TGT" a b c
finish

begin "target is a one-file subset of source"
mkfiles "$SRC" a b c
mkfiles "$TGT" b
finish

begin "source has one file, target has many (none shared)"
mkfiles "$SRC" m
mkfiles "$TGT" a b c x y z
finish

begin "target has one file, source has many (none shared)"
mkfiles "$SRC" a b c x y z
mkfiles "$TGT" m
finish

# ---- empty lists
begin "target empty, source has files"
mkfiles "$SRC" a b c
finish

begin "source empty, target has files"
mkfiles "$TGT" a b c
finish

begin "both empty"
finish

begin "target empty, source has a single file"
mkfiles "$SRC" a
finish

begin "source empty, target has a single file"
mkfiles "$TGT" a
finish

# ---- bigger lists
begin "50 files, partially overlapping"
for i in $(seq -w 1 50); do
    if [ $((10#$i % 2)) -eq 0 ] || [ $((10#$i % 3)) -eq 0 ]; then mkf "$SRC/f$i"; fi
    if [ $((10#$i % 3)) -eq 0 ] || [ $((10#$i % 5)) -eq 0 ]; then mkf "$TGT/f$i"; fi
done
finish

begin "200 identical files"
for i in $(seq -w 1 200); do
    mkf "$SRC/f$i"
    mkf "$TGT/f$i"
done
finish

summary
