#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CPUS number_of_processors

typedef struct
{
    long num_cpus;
    pthread_mutex_t glock;
    int global;
    pthread_mutex_t *lock;
    int *local;
    int treshold;
} counter;

typedef struct
{
    int amnt;
    int thread_id;
    counter *c;
} myargs;

void counter_init(counter *c, int treshold, int num_threads)
{
    c->num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    c->treshold = treshold;
    c->global = 0;

    assert(pthread_mutex_init(&c->glock, NULL) == 0);

    c->local = malloc(c->num_cpus * num_threads);

    for (int i = 0; i < c->num_cpus; i++)
    {
        c->local[i] = 0;
        assert(pthread_mutex_init(&c->lock[i], NULL) == 0);
    }
}

void counter_destroy(counter *c)
{
    pthread_mutex_destroy(&c->glock);
    for (int i = 0; i < c->num_cpus; i++)
    {
        pthread_mutex_destroy(&c->lock[i]);
    }
}

void *update(void *args)
{
    myargs *a = (myargs *)args;
    int amnt = a->amnt;
    int thread_id = a->thread_id;
    counter *c = a->c;
    assert(pthread_mutex_lock(&c->lock[thread_id]) == 0);
    c->local[thread_id] += amnt;
    if (c->local[thread_id] >= c->treshold)
    {
        assert(pthread_mutex_lock(&c->glock) == 0);
        c->global += c->local[thread_id];
        assert(pthread_mutex_unlock(&c->glock) == 0);
        c->local[thread_id] = 0;
    }
    assert(pthread_mutex_unlock(&c->lock[thread_id]) == 0);
    return NULL;
}

int main(int argc, char *argv[])
{
    long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

    printf("ENV: number_of_processors = %ld\n", number_of_processors);

    while ((opt = getopt(argc, argv, "s:t:o:")) != -1)
    {
        switch (opt)
        {
        case /* constant-expression */:
            /* code */
            break;

        default:
            break;
        }

        counter *c = malloc(sizeof(counter));
        counter_init(c, 5, 2);
        assert(c != NULL);
        myargs arg, arg1;
        arg.c = arg1.c = c;
        arg.amnt = arg1.amnt = 1;
        arg.thread_id = 0;
        arg1.thread_id = 1;
        pthread_t p1, p2;
        assert(pthread_create(&p1, NULL, update, (void *)&arg) == 0);
        assert(pthread_create(&p2, NULL, update, (void *)&arg1) == 0);
        assert(pthread_join(p1, NULL) == 0);
        assert(pthread_join(p2, NULL) == 0);

        free(c);
        return 0;
    }