//// DIRECTORY LOGIC ////
// If the source and target have the same name...
if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) == 0) &&
    // and both are directories...
    (source_ptr->fts_info == FTS_D) && (target_ptr->fts_info == FTS_D) &&
    // and the source was edited more recently than the target
    (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime)) {
        // overwrite the source directory into the target directory
        char *source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
        char *target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
        printf("Overwriting directory '%s' to directory '%s\n", source_full_path, target_full_path);
        directory_copy(source_full_path, target_full_path);
}
// If the target directory's name doesn't match the source directory's name, delete the target directory
if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) != 0) &&
    (source_ptr->fts_info == FTS_D) && (target_ptr->fts_info == FTS_D)) {
    char *full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
    printf("Normal directory delete for directory: '%s'\n\n", full_path);
    directory_delete(full_path);
}


//// FILE LOGIC ////
// If the source and target have the same name...
if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) == 0) &&
    // and both are files...
    (source_ptr->fts_info == FTS_F) && (target_ptr->fts_info == FTS_F) &&
    // and the source was edited more recently than the target
    (source_ptr->fts_statp->st_mtime > target_ptr->fts_statp->st_mtime)) {
        // overwrite the source file into the target file
        char *source_full_path = file_path_concatenate(arena_scratch, source_ptr->fts_path, source_ptr->fts_name);
        char *target_full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
        printf("Overwriting file '%s' to file '%s\n", source_full_path, target_full_path);
        file_copy(source_full_path, target_full_path);
}
// If the target file's name doesn't match the source file's name, delete the target file
if ((strcmp(source_ptr->fts_name, target_ptr->fts_name) != 0) &&
    (source_ptr->fts_info == FTS_F) && (target_ptr->fts_info == FTS_F)) {
    char *full_path = file_path_concatenate(arena_scratch, target_ptr->fts_path, target_ptr->fts_name);
    printf("Normal file delete for file: '%s'\n\n", full_path);
    file_delete(full_path);
}


// If the source and target directory names are different before
// reaching excess directories, I'm not sure what to do yet. Likely
// just a simple delete
// The prior logic also might ensure that doesn't happen

//// EXTRA FILE LOGIC ////
// TODO: ADD LOGIC FOR IF THE SOURCE DIRECTORY IS LONGER THAN THE TARGET
// this should follow the below logic
// Delete extra directories and files found in the target directory that are not found in the source directory
if ((source_ptr->fts_link == NULL) && (target_ptr->fts_link != NULL)) {
    FTSENT *excess_file = target_ptr->fts_link;
    while (excess_file != NULL) {
        // Create a full file path and arena checkpoint
        char *excess_file_path = file_path_concatenate(arena_scratch, excess_file->fts_path, excess_file->fts_name);
        // If it's a file, just delete the file
        if (excess_file->fts_info == FTS_F) {
            printf("Excess file delete for file: %s\n", excess_file_path);
            file_delete(excess_file_path);
        }
        // Otherwise, if it's a directory, recursively delete the directory
        else if (excess_file->fts_info == FTS_D) {
            printf("Excess directory delete for directory: %s\n", excess_file_path);
            directory_delete(excess_file_path);
        }
        // Otherwise, we don't know what the heck we're dealing with
        // (this error handling could almost definitely be improved
        // using the `stat` struct. Let's just get things moving for now)
        else {
            printf("ERROR: Unrecognized file type for file: '%s'\n", excess_file_path);
            return;
        }
        excess_file = excess_file->fts_link;
    }
}
