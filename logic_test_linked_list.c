#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char file;
    struct Node *next;
} Node;

Node *create_linked_list(char files[], size_t num_of_files);
void print_linked_list(Node *n);
void show_difference(Node *source, Node *target);

int main(void) {
    // Test for if a middle file is missing in the target
    // when source is larger than target
    char source_files[] = {'a', 'b', 'c'};
    char target_files[] = {'a', 'c'};
    size_t source_size = sizeof(source_files) / sizeof(source_files[0]);
    size_t target_size = sizeof(target_files) / sizeof(target_files[0]);
    Node *source_head = create_linked_list(source_files, source_size);
    Node *target_head = create_linked_list(target_files, target_size);

    printf("Source: ");
    print_linked_list(source_head);

    printf("Target: ");
    print_linked_list(target_head);

    printf("\n");

    show_difference(source_head, target_head);


    // Test for if a middle file is missing in the source
    // when target is larger than the source
}

Node *create_linked_list(char files[], size_t num_of_files) {
    Node *head = NULL, *tail = NULL;
    for (size_t i = 0; i < num_of_files; i++) {
        Node *n = malloc(sizeof(Node));
        if (n == NULL) {
            printf("Error creating `Node`\n");
            return NULL;
        }
        n->file = files[i];
        n->next = NULL;
        if (tail == NULL) {
            head = n;
        } else {
            tail->next = n;
        }
        tail = n;
    }

    return head;
}

void print_linked_list(Node *n) {
    Node *ptr = n;
    while (ptr != NULL) {
        printf("%c ", ptr->file);
        ptr = ptr->next;
    }
    printf("\n");
}

void show_difference(Node *source, Node *target) {
    Node *source_ptr = source;
    Node *target_ptr = target;
    while (source_ptr != NULL) {
        printf("Current source: %c\n", source_ptr->file);
        printf("Current target: %c\n", target_ptr->file);

        // 0 for false, 1 for true
        int is_different = 0;

        // If the source is not equal to the target, let the user know
        if (source_ptr->file == target_ptr->file) {
            is_different = 0;
        }
        else if (source_ptr->file != target_ptr->file) {
            is_different = 1;
        }

        if (is_different == 0) {
            printf("Source file '%c' is the same as target file '%c'\n", source_ptr->file, target_ptr->file);
            source_ptr = source_ptr->next;
            target_ptr = target_ptr->next;
        }
        else if (is_different == 1) {
            printf("Source file '%c; is different than target file '%c'\n", source_ptr->file, target_ptr->file);
            source_ptr = source_ptr->next;
            target_ptr = target_ptr->next;
        }

        if (source_ptr->next == NULL) {
            printf("\nReached end of source list at file '%c'\n", source_ptr->file);
        }
        if (target_ptr->next == NULL) {
            printf("\nReached end of target list at file '%c'\n", target_ptr->file);
        }

        printf("\n");
    }
}
