#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>

#define DEF_SIZE 10
#define DEF_THREADS 2

char *orderstr[] = {"none", "pre", "post"};

typedef struct __node__
{
    struct __node__ *prev;
    struct __node__ *next;
    pthread_mutex_t lock;
    int *value;
} node;

void node_init(node *head, int *val)
{
    head->prev = NULL;
    head->next = NULL;
    head->value = val;
    assert(pthread_mutex_init(&head->lock, NULL) == 0);
}

node *node_create(int *val)
{
    node *p = malloc(sizeof(node));
    node_init(p, val);
    return p;
}

node *list_add(node *head, int *val)
{
    pthread_mutex_lock(&head->lock);
    if (head->value == NULL)
    {
        head->value = val;
    }
    else if (head->next == NULL)
    {
        head->next = node_create(val);
    }
    else if (*head->value == *val)
    {
        pthread_mutex_unlock(&head->lock);
        return head;
    }
    else if (*head->value < *val)
    {
        head->next = list_add(head->next, val);
    }
    else if (*head->value > *val)
    {
        node *new_head = node_create(val);
        new_head->next = head;
        pthread_mutex_unlock(&head->lock);
        return new_head;
    }
    pthread_mutex_unlock(&head->lock);
    return head;
}

int *list_get(node *head, int index)
{
    node *p = head;
    for (int i = 0; i < index; i++)
    {
        p = p->next;
    }
    assert(p != NULL);
    return p->value;
}

void list_postorder_trav(node *head, void (*nodefun)(node *))
{
    if (head->next != NULL)
    {
        list_postorder_trav(head->next, nodefun);
    }

    nodefun(head);
}

void list_preorder_trav(node *head, void (*nodefun)(node *))
{
    nodefun(head);

    if (head->next != NULL)
    {
        list_postorder_trav(head->next, nodefun);
    }
}

void node_del(node *n)
{
    pthread_mutex_destroy(&n->lock);
    free(n);
}

void list_print(node *n)
{
    pthread_mutex_lock(&n->lock);
    printf("Node %d\n", *n->value);
    pthread_mutex_unlock(&n->lock);
}

typedef struct
{
    int a;
    int z;
    node *head;
    int *numbers;
} arg_t;

void *inserter(void *arg)
{
    arg_t *a = (arg_t *)arg;
    for (int i = a->a; i < a->z; i++)
    {
        list_add(a->head, (a->numbers + i));
    }
    return NULL;
}

void *printer(void *arg)
{
    arg_t *a = (arg_t *)arg;
    for (int i = a->a; i < a->z; i++)
    {
        printf("Node %d\n", *list_get(a->head, i));
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int list_size = DEF_SIZE;
    int num_threads = DEF_THREADS;
    int print_order = 0;
    int num_cpus = sysconf(_SC_NPROCESSORS_CONF);

    printf("\nArguments (ARG) and Environment(ENV):\n");
    printf("\tENV num_cpus = %d\n", num_cpus);

    int opt = 0;

    while ((opt = getopt(argc, argv, "s:t:o:")) != -1)
    {
        switch (opt)
        {
        case 's':
            list_size = atoi(optarg);
            printf("\tARG list_size = %d\n", list_size);
            break;
        case 't':
            num_threads = atoi(optarg);
            printf("\tARG num_threads = %d\n", num_threads);
            break;
        case 'o':

            if (strcmp(optarg, orderstr[1]) == 0)
            {
                print_order = 1;
            }
            else if (strcmp(optarg, orderstr[2]) == 0)
            {
                print_order = 2;
            }
            printf("ARG print_order = %d (%s)\n", print_order, orderstr[print_order]);
            break;
        default:
            break;
        }
    }
    printf("\n");

    node *head = node_create(NULL);
    node_init(head, NULL);

    int *numbers = malloc(list_size * sizeof(int));

    for (int i = 0; i < list_size; i++)
    {
        *(numbers + i) = i;
    }

    arg_t *args = malloc(num_threads * sizeof(arg_t));

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    for (int i = 0; i < num_threads; i++)
    {
        // initialize arguments[i] for thread[i]
        (args + i)->a = i;
        (args + i)->z = i + list_size / num_threads - 1;
        (args + i)->head = head;
        (args + i)->numbers = numbers;

        // create thread[i] with arguments[i]
        pthread_create((threads + i), NULL, inserter, (args + i));
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(*(threads + i), NULL);
    }

    gettimeofday(&t1, NULL);

    printf("Elapsed time for insertion:\n%ld s\n%ld ms\n", (t1.tv_sec - t0.tv_sec), (t1.tv_usec - t0.tv_usec));

    if (print_order == 1)
    {
        printf("\nprint list in preorder:\n");
        list_preorder_trav(head, list_print);
    }
    else if (print_order == 2)
    {
        printf("\nprint list in postorder:\n");
        list_postorder_trav(head, list_print);
    }

    list_postorder_trav(head, node_del);
    free(numbers);
    return 0;
}