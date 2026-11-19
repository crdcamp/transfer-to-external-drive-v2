#!/bin/bash

mkdir Target
cd Target

mkdir SubDir1
cd SubDir1
touch creepycrawly.md test1.c test2.c test3.c
cd ..

mkdir SubDir2
cd SubDir2
touch test1.c test2.c test3.c
cd ..

mkdir SubDir3
cd SubDir3
touch test1.c test2.c test3.c
cd ..
