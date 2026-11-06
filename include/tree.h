typedef struct Node {
    char *name;
    struct Node *parent;
    struct Node *first_child;
    struct Node *next_sibling;
    int is_dir; // 0 for false, 1 for true
} Node;

Node *tree_create_test(char *root_path);
