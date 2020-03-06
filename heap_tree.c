#include<stdio.h>
#include<stdlib.h>

#define HEAP_SIZE 16 * 1024

char heap[HEAP_SIZE];

struct memory_block {
    char * start;
    int size;
    enum {OCCUPIED, FREE, SPLIT} state;
};

struct memory_node {
    struct memory_block block;
    struct memory_node * left;
    struct memory_node * right;
};

typedef struct memory_block memory_block;
typedef struct memory_node memory_node;

memory_block new_block(char * start_addr, int size);
void print_block(memory_block m);

int next_higher_power(int n) {
    int m = 1;
    while(m < n) {
        m = m << 1;
    }
    return m;
}

char * get_state(memory_block b) {
    switch(b.state) {
        case 0:
            return "OCCUPIED";
        case 1:
            return "FREE";
        case 2:
            return "SPLIT";
    }
}

memory_node * init_tree() {
    // Set the entire heap to NULL
    for(int i = 0; i < HEAP_SIZE; i++) {
        heap[i] = 0;
    }

    // Allocate memory for the root node
    memory_node * root = malloc(sizeof(memory_node));
    // Assign to it the root block
    root->block = new_block(heap, HEAP_SIZE);
    // Make it a terminal node
    root->left = NULL;
    root->right = NULL;

    return root;
};

void print_tree(memory_node * root);

void split(memory_node * parent);

// Caution: Use only on terminal nodes
void merge_children(memory_node * parent);
// collapses all free children of given node
void collapse(memory_node * tree);

int allocate(memory_node * tree, int size, memory_node ** allocated);
void free_node(memory_node * m);

int main() {
    memory_node * root = init_tree();
    memory_node * blocks[3];
    allocate(root, 123, &blocks[0]);
    allocate(root, 1123, &blocks[1]);
    print_tree(root);
    blocks[0]->block.state = FREE;
    blocks[1]->block.state = FREE;
    collapse(root);
    print_tree(root);
    return 0;
}

memory_block new_block(char * start_addr, int size) {
    memory_block m;
    m.start = start_addr;
    m.size = size;
    m.state = FREE;
    return m;
}

void print_block(memory_block m) {
    printf("|Block start: %p\t Block size: %d\t Block state: %s\n", m.start, m.size, get_state(m));
}

void print_tree(memory_node * root) {
    if (root == NULL) {
        return;
    } else {
        printf("+-------\n");
        printf("|Node Address: %p\t Left: %p\t Right: %p\n", root, root->left, root->right);
        print_block(root->block);
        printf("+-------\n");
        printf("\n");
        print_tree(root->left);
        print_tree(root->right);
    }
}

void split(memory_node * parent) {
    if(parent->left || parent->right) {
        fprintf(stderr, "ERROR: Unable to split node.\n");
    } else {
        memory_node * lc = malloc(sizeof(memory_node));
        memory_node * rc = malloc(sizeof(memory_node));

        memory_block temp = parent->block;
        
        lc->block = temp;
        lc->block.size = (lc->block.size / 2);

        rc->block = temp;
        rc->block.size = (rc->block.size / 2);
        rc->block.start = lc->block.start + lc->block.size;

        parent->left = lc;
        parent->right = rc;

        parent->block.state = SPLIT;
    }
}

void merge_children(memory_node * parent) {
    // Free memory allocated to the children
    free(parent->left);
    free(parent->right);

    // Set children to null
    parent->left = NULL;
    parent->right = NULL;
}

void collapse(memory_node * tree) {
    // If you are at a terminal node
    if (tree->left == NULL && tree->right == NULL) {
        // Go back
        return;
    } else {
        // Collapse all to the left and right
        collapse(tree->left);
        collapse(tree->right);
        // If the left child is free and the right child is feee
        if (tree->left->block.state == FREE && tree->right->block.state == FREE) {
            // And the current block is marked as split
            if (tree->block.state == SPLIT) {
                // Mark current block as free
                tree->block.state = FREE;
            }
            // Merge both of its children
            merge_children(tree);
        }
    }
}

int allocate(memory_node * tree, int size, memory_node ** allocated) {
    // get the precise block size to allocate
    int sz = next_higher_power(size);
    // If the given block is smaller than size
    if (tree->block.size < sz) {
        // Return 0 because it cannot be allocated
        return 0;
    } else {
        if (tree->block.size == sz) {
            // If the best fir block is found
            if (tree->block.state == FREE) {
                // Set its state to occupied and return 1
                *allocated = tree;
                tree->block.state = OCCUPIED;
                return 1;
            } else {
                // Return 0 if a block is found but not free
                return 0;
            }
        } else {
            // If you reach an end node of larger size
            if (tree->left == NULL && tree->right == NULL) {
                // Split the given node in two
                split(tree);
            }
            // If allocation succeeds on the left branch    
            if (allocate(tree->left, size, allocated)) {
                // Return 1
                return 1;
            } else {
                // Try allocating on the right
                return allocate(tree->right, size, allocated);
            }
        }
    }
}

void free_node(memory_node * m) {
    m->block.state = FREE;
}