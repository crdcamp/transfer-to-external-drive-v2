#include "../include/arena.h"
#include "../include/sync.h"

int main(void) {
    // Create a scratch arena
    size_t arena_scratch_buffer_size = PATH_MAX*128;
    Arena arena_scratch = {0};
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);

    // Define test directories and test the sync function
    char *source = "TestDirs/Source";
    char *target = "TestDirs/Target";

    test_directories_sync(&arena_scratch, source, target);

    return 0;
}
