#include <string.h>
#include "include/tree_arena.h"

size_t arena_strlen(const char *s)
{
    size_t n = 0;
    while (*s++) n++;
    return n;
}

int main(void) {
    char *example_string = "meow";

    size_t str_len_len = strlen(example_string);
    size_t arena_str_len_len = arena_strlen(example_string);

    printf("`strlen` result: %zu\n", str_len_len);
    printf("arena result: %zu\n", arena_str_len_len);
}
