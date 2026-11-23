# Deleting a single file

# Deleting a directory

# Copying a single file

```c
FTSENT *s = source_children;
while (s != NULL) {
    char *source_file = file_path_concatenate(arena_scratch, s->fts_path, s->fts_name);
    char *target_file = file_path_concatenate(arena_scratch, target_directory, s->fts_name);
    printf("Copying file `%s`\ninto target  `%s`\n\n", source_file, target_file);
    file_copy(source_file, target_file);

    s = s->fts_link;
}
```

# Copying a Directory

```c

```
