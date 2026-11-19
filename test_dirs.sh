cd TestDirs

# Target1: identical copy (nothing should change)
cp -r Source Target1

# Target2: empty target (everything must be created)
mkdir Target2

# Target3: extra files in existing subdirs (should be deleted)
cp -r Source Target3
touch Target3/SubDir1/extra1.c Target3/SubDir2/extra2.c Target3/SubDir2/extra3.c

# Target4: extra directories, one empty, one with nested contents (tests your recursive delete)
cp -r Source Target4
mkdir Target4/SubDir4 Target4/EmptyDir
mkdir -p Target4/SubDir5/Nested/Deeper
touch Target4/SubDir5/a.c Target4/SubDir5/Nested/b.c Target4/SubDir5/Nested/Deeper/c.c

# Target5: missing files and a missing directory (should be created)
cp -r Source Target5
rm Target5/SubDir1/test2.c Target5/SubDir2/test3.c
rm -r Target5/SubDir3

# Target6: files with the same names but different contents (tests update detection)
cp -r Source Target6
echo "different" > Target6/SubDir1/test1.c
echo "different" > Target6/SubDir3/test3.c

# Target7: type mismatch (file where a directory should be, directory where a file should be)
cp -r Source Target7
rm -r Target7/SubDir2
touch Target7/SubDir2
rm Target7/SubDir1/test1.c
mkdir Target7/SubDir1/test1.c

# Target8: mixed (missing, extra, and different all at once)
cp -r Source Target8
rm Target8/SubDir1/test3.c
echo "different" > Target8/SubDir2/test2.c
touch Target8/SubDir3/extra.c
mkdir -p Target8/ExtraDir/Inner
touch Target8/ExtraDir/Inner/x.c
