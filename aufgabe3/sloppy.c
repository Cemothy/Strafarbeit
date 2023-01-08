#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CPUS 6

typedef struct
{
    pthread_mutex_t *glock;
    int global;
    pthread_mutex_t lock[NUM_CPUS];
    int local[NUM_CPUS];
    int treshold;
} counter;

typedef struct
{
    int amnt;
    int thread_id;
    counter *c;
} myargs;

void init(counter *c, int treshold)
{
    c->treshold = treshold;
    c->global = 0;
    assert(pthread_mutex_init(c->glock, NULL) == 0);
    for (int i = 0; i < NUM_CPUS; i++)
    {
        c->local[i] = 0;
        assert(pthread_mutex_init(&c->lock[i], NULL) == 0);
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
        assert(pthread_mutex_lock(c->glock) == 0);
        c->global += c->local[thread_id];
        assert(pthread_mutex_unlock(c->glock) == 0);
        c->local[thread_id] = 0;
    }
    assert(pthread_mutex_unlock(&c->lock[thread_id]) == 0);
    return NULL;
}

int main(void)
{
    counter *c = malloc(sizeof(counter));
    init(c, 5);
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