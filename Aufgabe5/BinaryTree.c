// Tree traversal in C
// Datastruct from: https://www.scaler.com/topics/binary-tree-in-c/

#include "BinaryTree.h"

// Inorder traversal
void inorderTraversal(struct node *root)
{
  if (root == NULL)
    return;
  inorderTraversal(root->left);
  printf("%d ", root->item);
  inorderTraversal(root->right);
}

// Preorder traversal
void preorderTraversal(struct node *root)
{
  if (root == NULL)
    return;
  printf("%d ", root->item);
  preorderTraversal(root->left);
  preorderTraversal(root->right);
}

// Postorder traversal
void postorderTraversal(struct node *root)
{
  if (root == NULL)
    return;
  postorderTraversal(root->left);
  postorderTraversal(root->right);
  printf("%d ", root->item);
}

// Create a new Node
struct node *create(int value)
{
  struct node *newNode = malloc(sizeof(struct node));
  newNode->item = value;
  newNode->left = NULL;
  newNode->right = NULL;

  return newNode;
}

void free_all_nodes(struct node *root)
{
  if (root == NULL)
    return;
  free_all_nodes(root->left);
  free_all_nodes(root->right);
  free(root);
}

binarytree *create_binary(binarytree *b)
{
  b->root = create(0);
  assert(pthread_mutex_init(&b->lock, NULL) == 0);
  return b;
}

void insert(binarytree *b, int value)
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
  else if (item > p->item)
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
  if (p == NULL)
  {
    return 0;
  }
  else if (p->item == n)
  {
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
  int a = arcs->a;
  binarytree *b = arcs->b;
  for (int i = a; i < n; i++)
  {
    insert(b, numbers[i]);
  }
  /*
  for (int i = a; i < n; i++)
  {
    contains(b, rand());
  }
  */
  return NULL;
}

int main(int argc, char **argv)
{

  // Parameter Eingabe
  long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
  int threads = 2;
  int options;
  int number = 1000;

  while ((options = getopt(argc, argv, "p:n:")) != -1)
  {
    switch (options)
    {
    case 'p':
      threads = atoi(optarg);
      break;
    case 'n':
      number = atoi(optarg);
      break;
    }
  }

  // Ausgeben der Parameter
  printf("CPUs: %ld\n", number_of_processors);
  printf("Threads: %d\n", threads);
  printf("Numbers: %d\n", number);

  // initialize randomizer
  srand((unsigned int)time(NULL));
  int *numbers = malloc(number * sizeof(int));
  for (int i = 0; i < number; i++)
  {
    numbers[i] = i;
  }

  // Binary Tree inizialisieren
  binarytree *b = malloc(sizeof(binarytree));
  create_binary(b);

  // Argumente für Worker in Struct einfügen
  myargs args[threads];


for(int i = 0; i < threads; i++)
{
  args[i].b = b;
  args[i].numbers = numbers;
  args[i].a = (number / threads) * i;
  args[i].n = (number / threads) * (i + 1);

}

  struct timeval t1, t2;
  // Threads erstellen
  pthread_t p[threads];
  gettimeofday(&t1, NULL);
  for (int i = 0; i < threads; i++)
  {

    assert(pthread_create(&p[i], NULL, worker, (void *)&args) == 0);
  }

  for (int i = 0; i < threads; i++)
  {

    assert(pthread_join(p[i], NULL) == 0);
  }
  gettimeofday(&t2, NULL);

  double time = (t2.tv_sec - t1.tv_sec) * 1000.0;
  time += (t2.tv_usec - t1.tv_usec) / 1000.0;

  printf("Elapsed Time: %f ms.\n", time);

  // free lock, array, nodes and tree
  pthread_mutex_destroy(&b->lock);
  free(numbers);
  free_all_nodes(b->root);
  free(b);
}
