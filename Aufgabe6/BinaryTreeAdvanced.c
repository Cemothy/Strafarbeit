// Tree traversal in C
// Datastruct from: https://www.scaler.com/topics/binary-tree-in-c/

#include "BinaryTreeAdvanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

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
	assert(pthread_mutex_init(&newNode->lockL, NULL) == 0);
	assert(pthread_mutex_init(&newNode->lockR, NULL) == 0);

	return newNode;
}

void free_all_nodes(struct node *root)
{
	if (root == NULL)
		return;
	pthread_mutex_destroy(&root->lockR);
	pthread_mutex_destroy(&root->lockL);
	free_all_nodes(root->left);
	free_all_nodes(root->right);
	free(root);
}

binarytree *create_binary(binarytree *b)
{
	b->root = create(0);
	// Removed Global Lock Init
	return b;
}

void insert(binarytree *b, int value)
{

	insertR(b->root, value);
}

/**
 * @brief insertR changes the node it's been given
 *
 * @param p Node
 * @param value
 */

void insertR(node *p, int item)
{

	if (item > p->item)
	{
		pthread_mutex_lock(&p->lockR);
		if (p->right == NULL)
		{
			p->right = create(item);
		}

		pthread_mutex_unlock(&p->lockR);

		if (p->right != NULL)
		{
			insertR(p->right, item);
		}
	}
	else if (item < p->item)
	{
		pthread_mutex_lock(&p->lockL);
		if (p->left == NULL)
		{
			p->left = create(item);
		}

		pthread_mutex_unlock(&p->lockL);

		if (p->left != NULL)
		{
			insertR(p->left, item);
		}
	}
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
	return NULL;
}

int main(int argc, char **argv)
{

	// Parameter Eingabe
	long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
	int threads = 4;
	int options;
	int number = 1000;
	bool print = 0;
	int print_to_file = 0;

	while ((options = getopt(argc, argv, "p:n:c:F:")) != -1)
	{
		switch (options)
		{
		case 'p':
			threads = atoi(optarg);
			break;
		case 'n':
			number = atoi(optarg);
			break;
		case 'c':
			print = 1;
			break;
		case 'F':
			print_to_file = atoi(optarg);
			if (print_to_file == 2)
			{
				FILE *file = fopen("out.csv", "a");
				assert(file != NULL);
				assert(0 < fprintf(file, "number, time\n"));
				fclose(file);
			}
			break;
		}
	}

	// Ausgeben der Parameter
	if (print_to_file == 0)
	{
		printf("CPUs: %ld\n", number_of_processors);
		printf("Threads: %d\n", threads);
		printf("Numbers: %d\n", number);
	}

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
	myargs *args = malloc(threads * sizeof(myargs));

	for (int i = 0; i < threads; i++)
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
		assert(pthread_create(&p[i], NULL, worker, (void *)&args[i]) == 0); // wichtig args mit [i]!!
	}

	for (int i = 0; i < threads; i++)
	{

		assert(pthread_join(p[i], NULL) == 0);
	}
	gettimeofday(&t2, NULL);

	double time = (t2.tv_sec - t1.tv_sec) * 1000.0;
	time += (t2.tv_usec - t1.tv_usec) / 1000.0;

	if (print_to_file == 0)
	{
		printf("Elapsed Time: %f ms.\n", time);
		FILE *file = fopen("out.txt", "a");
		assert(file != NULL);
		assert(0 < fprintf(file, "Number: %d\t Elapsed Time: %f\n", number, time));
		fclose(file);
	}
	else
	{

		FILE *file = fopen("out.csv", "a");
		assert(file != NULL);
		assert(0 < fprintf(file, "%d, %f;\n", number, time));
		fclose(file);
	}

	// Plot
	/*
	double x [] = {100, 1000, 10000, 100000};  //numbers
	double y [] = {0,10,100,1000}; // elapsed time

	RGBABitmapImageReference *imageRef = CreateRGBABitmapImageReference();

	DrawScatterPlot(imageRef, 600, 400, x, 4, y, 4, NULL);

	size_t length;
	double *pngData = ConvertToPNG(&length, imageRef->image);
	WriteToFile(pngData, length, "plot.png");
	*/
	// End Plot

	if (print)
	{
		inorderTraversal(b->root);
	}
	// free lock, array, nodes and tree

	free(numbers);
	free_all_nodes(b->root);
	free(b);
	free(args);
}
