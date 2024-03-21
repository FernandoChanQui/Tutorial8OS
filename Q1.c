#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Proc {
    char parent[256];
    char name[256];
    int priority;
    int memory;
    struct Proc *left;
    struct Proc *right;
};

struct ProcTree {
    struct Proc *root;
};

struct Proc* create_proc_node(char *parent, char *name, int priority, int memory) {
    struct Proc* new_proc = (struct Proc*)malloc(sizeof(struct Proc));
    if (new_proc == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_proc->parent, parent);
    strcpy(new_proc->name, name);
    new_proc->priority = priority;
    new_proc->memory = memory;
    new_proc->left = NULL;
    new_proc->right = NULL;
    return new_proc;
}

void insert_proc(struct Proc **root, struct Proc *new_proc) {
    if (*root == NULL) {
        *root = new_proc;
    } else {
        if (strcmp(new_proc->parent, (*root)->name) <= 0) {
            insert_proc(&((*root)->left), new_proc);
        } else {
            insert_proc(&((*root)->right), new_proc);
        }
    }
}

void print_inorder(struct Proc *root) {
    if (root != NULL) {
        print_inorder(root->left);
        printf("Parent: %-10s Name: %-10s Priority: %-5d Memory: %-5dMB\n", root->parent, root->name, root->priority, root->memory);
        print_inorder(root->right);
    }
}

int main() {
    FILE *file = fopen("processes_tree.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    struct ProcTree proc_tree;
    proc_tree.root = NULL;

    char parent[256];
    char name[256];
    int priority;
    int memory;
    while (fscanf(file, "%[^,], %[^,], %d, %d\n", parent, name, &priority, &memory) != EOF) {
        struct Proc *new_proc = create_proc_node(parent, name, priority, memory);
        insert_proc(&proc_tree.root, new_proc);
    }
    fclose(file);

    printf("Binary Tree Contents:\n");
    print_inorder(proc_tree.root);

    return 0;
}





