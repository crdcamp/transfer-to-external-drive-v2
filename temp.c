#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *strings[] = {"meow", "beep", "shebangyo"};
    size_t strings_len = sizeof(strings)/sizeof(strings[0]);
    size_t strings_size = 1;
    char *strings_cp = NULL;
    for (size_t i = 0; i < strings_len; i++) {
        strings_size += strlen(strings[i]);
        char *strings_cp = malloc(strings_size);

        printf("size of string '%s': %zu\n", strings[i], strlen(strings[i]));
    }
    printf("Total size (including null): %zu\n", strings_size);


    // Now allocate the)

    return 0;
}
