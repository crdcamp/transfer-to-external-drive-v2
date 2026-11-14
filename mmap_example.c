#include <sys/mman.h>

void *buf = mmap(NULL, size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANON,
                 -1, 0);
if (buf == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

arena_init(&arena, buf, size);
// ...
munmap(buf, size);

Arena arena;
arena_create(&arena, 1 << 20);  // 1 MB

Node *source_root = new_node(&arena, source_dir);
Node *target_root = new_node(&arena, target_dir);
tree_fill(&arena, &temp, source_root, source_dir);
tree_fill(&arena, &temp, target_root, target_dir);
