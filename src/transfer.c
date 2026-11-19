#include "../include/arena.h"
#include "../include/sync.h"

int main(void) {
    // Create scratch and main arenas
    size_t arena_buffer_size = 131072;
    size_t arena_scratch_buffer_size = PATH_MAX*4;

    Arena arena = {0};
    Arena arena_scratch = {0};

    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_scratch_buffer[arena_scratch_buffer_size];
    _Alignas(DEFAULT_ALIGNMENT) unsigned char arena_buffer[arena_buffer_size];

    size_t arena_scratch_buffer_length = sizeof(arena_scratch_buffer) / sizeof(arena_scratch_buffer[0]);
    size_t arena_buffer_length = sizeof(arena_buffer) / sizeof(arena_buffer[0]);

    arena_init(&arena, arena_buffer, arena_buffer_length);
    arena_init(&arena_scratch, arena_scratch_buffer, arena_scratch_buffer_length);


    char *source[] = {"TestDirs/Source", NULL};
    char *target[] = {"TestDirs/Target", NULL};

    sync_directories(&arena_scratch, source, target);

    return 0;
}
