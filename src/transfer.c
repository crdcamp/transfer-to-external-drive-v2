#include "../include/arena.h"
#include "../include/sync.h"

// RENAME THIS FILE TO `sync.c` AND RENAME `sync.c` to `sync_directories.`
// After all, this is no longer a basic file transfer. This is a way more complex directory sync

int main(void) {
    // Create a scratch arena
    size_t arena_scratch_buffer_size = PATH_MAX*64; // This is an excessive amount of space, but we can narrow it down later
    Arena arena_scratch = {0};
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);

    // Define test directories and test the sync function
    char *source = "TestDirs/Source";
    char *target = "TestDirs/Target";

    sync_directories(&arena_scratch, source, target);

    return 0;
}
