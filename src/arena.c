#include "../include/arena_tree.h"
#include <string.h>

// Initialize and empty arena
void arena_init(Arena *arena, void *buffer, size_t buffer_length) {
    arena->buffer = buffer;
    arena->buffer_length = buffer_length;
    arena->previous_offset = 0;
    arena->current_offset = 0;
}

// Very self explainable
bool is_power_of_two(uintptr_t x) {
    // Fancy bitwise operations
    return (x & (x-1)) == 0;
}

// Move a pointer forward to an aligned memory address
uintptr_t align_forward(uintptr_t ptr, size_t align) {
    uintptr_t p, a, modulo;

    assert(is_power_of_two(align));

    p = ptr;
    a = align;
    // Same as using `%`, but faster since a is a power of 2
    modulo = p & (a-1);
    if (modulo != 0) {
        // Taking the difference between the align and remainder
        // brings us to an aligned memory address
        p += a - modulo;
    }

    return p;
}

// Allocate memory to the arena while ensuring memory alignment
void *arena_allocate(Arena *arena, size_t size, size_t align) {
    // Set an offset to an aligned memory location
    uintptr_t curr_ptr = (uintptr_t)arena->buffer + arena->current_offset;
    uintptr_t offset = align_forward(curr_ptr, align);
    offset -= (uintptr_t)arena->buffer; // Change to relative offset

    // Check if the arena has enough memory to allocate
    if (offset+size <= arena->buffer_length) {
        // Set a pointer to where the new allocation begins
        void *ptr = &arena->buffer[offset];
        // Set the previous offset to the new relative offset
        arena->previous_offset = offset;
        // Set the current offset to the end of the newly allocated memory
        arena->current_offset = offset+size;

        return ptr;
    }

    // If there's not enough memory to allocate, return NULL
    return NULL;
}

// Allocates a null-terminated string to the arena and returns a pointer to the resulting string
void *arena_string_store(Arena *arena, char *string, size_t string_size) {
    // Allocate the string's size in bytes to the arena
    void *string_ptr = arena_allocate(arena, string_size, DEFAULT_ALIGNMENT);
    if (string_ptr == NULL) {
        return NULL;
    }
    // Copy the string's data to the arena
    memcpy(string_ptr, string, string_size);

    return string_ptr;
}

// Same as `strdup` only it uses an arena instead of `malloc`
void *arena_string_duplicate(Arena *arena, char *string, size_t string_size) {
    void *string_ptr = arena_allocate(arena, string_size, DEFAULT_ALIGNMENT);
    if (string_ptr == NULL) return NULL;
    memcpy(string_ptr, string, string_size);

    return string_ptr;
}

// Reset the offsets for a given string
// Still questioning whether this will be useful later on...
// Also, this was AI generated, so might wanna revisit it just in case
// cause it looks like it could be a bit more readable
void arena_string_reset(Arena *arena, void *string_ptr, size_t string_size) {
    size_t start = (size_t)((byte *)string_ptr - arena->buffer);
    assert(start == arena->previous_offset);
    assert(start + string_size == arena->current_offset);

    arena->current_offset = start;
    arena->previous_offset = start;
}

void arena_reset(Arena *arena) {
    arena->current_offset = 0;
    arena->previous_offset = 0;
}
