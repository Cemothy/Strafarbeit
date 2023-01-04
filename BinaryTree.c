// Tree traversal in C
//Datastruct from: https://www.scaler.com/topics/binary-tree-in-c/

#include"BinaryTree.h"

// Inorder traversal
void inorderTraversal(struct node* root) {
  if (root == NULL) return;
  inorderTraversal(root->left);
  printf("%d ", root->item);
  inorderTraversal(root->right);
}

// Preorder traversal
void preorderTraversal(struct node* root) {
  if (root == NULL) return;
  printf("%d ", root->item);
  preorderTraversal(root->left);
  preorderTraversal(root->right);
}

// Postorder traversal
void postorderTraversal(struct node* root) {
  if (root == NULL) return;
  postorderTraversal(root->left);
  postorderTraversal(root->right);
  printf("%d ", root->item);
}

// Create a new Node
struct node* create(int value) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->item = value;
  newNode->left = NULL;
  newNode->right = NULL;

  return newNode;
}


void postorderDeletion(struct node* root)
{
  if (root == NULL) return;
  postorderTraversal(root->left);
  postorderTraversal(root->right);
  printf("free(%d)\n", root->item);
  free(root);
}


binarytree *create_binary(binarytree* b)
{
  b->root = create(0);
  assert(pthread_mutex_init(&b->lock, NULL) == 0);
  return b;
}


void insert(binarytree* b, int value)
{
  pthread_mutex_lock(&b->lock);
  insertR(b->root, value);
  pthread_mutex_unlock(&b->lock);
}

/**
 * @brief insertR changes the node it's been given
 *
 * @param p Node
 * @param value
 * @return node* if p->value was equal to value, p stays the same
 */

node *insertR(node *p, int item)
{
  if (p == NULL)
  {
    create(item);
  }
  else if(item > p->item)
  {
    p->right = insertR(p->right, item);
  }
  else if (item < p->item)
  {
    p->left = insertR(p->left, item);
  }

  return p;

}
/**
 * @brief
 *
 * @param b
 * @param n
 * @return 0 if not contained, 1 if contained
 */

int contains(binarytree *b, int n)
{
  return containsR(b->root, n);
}

int containsR(node *p, int n)
{
  if (p == NULL) {
    return 0;
  }
  else if (p->item == n) {
    return 1;
  }
  else if (p->item < n)
  {
    return containsR(p->right, n);
  }
  else if (p->item > n)
  {
    return containsR(p->left, n);
  }
  return 0;
}



void *worker(void *args)
{
  myargs *arcs = (myargs *)args;
  int *numbers = arcs->numbers;
  int n = arcs->n;
  binarytree* b = arcs->b;
  for (int i = 0; i < n; i++)
  {
    insert(b, numbers[i]);
  }
  for (int i = 0; i < n; i++)
  {
    contains(b, rand());
  }
  return NULL;
}

int main(int argc, char** argv) {
  long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
  printf("CPUs: %ld\n", number_of_processors);

  int threads = 2;
  int options;
  int number = 1000;

  while((options = getopt(argc,argv,"p:n:"))!= -1)
  {
    switch(options)
    {
      case 'p':
      threads = atoi(optarg);
      break;
      case 'n':
      number = atoi(optarg);
      break;
    }
  }

  // initialize randomizer
  srand((unsigned int) time(NULL));

  int *numbers = malloc(number * sizeof(int));
  for (int i = 0; i < number; i++)
  {
    numbers[i] = rand();
  }


  binarytree* b = malloc(sizeof(binarytree));
  create_binary(b);

  printf("Threads: %d\n", threads);
  printf("Numbers: %d\n", number);

  pthread_t p[threads];

  myargs args;
  args.b = b;
  args.numbers = numbers;
  args.n = number;

  for (int i = 0; i < threads; i++)
    {
        assert(pthread_create(&p[i], NULL, worker, (void *)&args) == 0);
    }

  for (int i = 0; i < threads; i++)
    {
      printf("Hier2\n");

      assert(pthread_join(p[i], NULL) == 0);
          printf("hurensohn\n");

    }
    printf("Hier\n");
    free(numbers);
    postorderDeletion(b->root);
    free(b);






/*
  printf("Traversal of the inserted binary tree \n");
  printf("Inorder traversal \n");
  inorderTraversal(b->root);

  printf("\nPreorder traversal \n");
  preorderTraversal(b->root);


  printf("\nPostorder traversal \n");
  postorderTraversal(b->root);
*/


}
