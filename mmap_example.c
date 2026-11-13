#include <sys/mman.h>

void *buf = mmap(NULL, size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANON,
                 -1, 0);
if (buf == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

arena_init(&arena, buf, size);
// ...
munmap(buf, size);
