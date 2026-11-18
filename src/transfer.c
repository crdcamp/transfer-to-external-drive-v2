#include "../include/arena.h"
#include "../include/compare.h"

int main(void) {
    // Scratch arena creation
    size_t arena_scratch_buffer_size = PATH_MAX*64;
    Arena arena_scratch = {0};
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];
    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);

    char *source[] = {"TestDirs/Source", NULL};
    char *target[] = {"TestDirs/Target", NULL};

    compare_directories(source, target);

    return 0;
}
