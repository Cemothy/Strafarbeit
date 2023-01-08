#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>


struct node {
    int item;
    struct node* left;
    struct node* right;
};

typedef struct node node;

typedef struct {
    node *root;
    pthread_mutex_t lock;
} binarytree;

typedef struct {
    binarytree *b;
    int *numbers;
    int a;
    int n;
} myargs;

void inorderTraversal(struct node* root);
void preorderTraversal(struct node* root);
void postorderTraversal(struct node* root);

struct node* create(int value);

binarytree *create_binary(binarytree* b);
void insert(binarytree* b, int value);

node *insertR(node *p, int value);

int contains(binarytree *b, int n);
int containsR(node *p, int n);

void *worker(void *);