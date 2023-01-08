#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct
{
    long value;
    pthread_mutex_t lock;
} counter;

typedef struct
{
    counter *c;
    long n;
} myargs;

void init(counter *c)
{
    c->value = 0;
    assert(pthread_mutex_init(&c->lock, NULL) == 0);
}

void increment(counter *c)
{
    assert(pthread_mutex_lock(&c->lock) == 0);
    c->value++;
    assert(pthread_mutex_unlock(&c->lock) == 0);
}

int get(counter *c)
{
    assert(pthread_mutex_lock(&c->lock) == 0);
    long rc = c->value;
    assert(pthread_mutex_unlock(&c->lock) == 0);
    return rc;
}

void *worker(void *arg)
{
    myargs *a = (myargs *)arg;
    long n = a->n;
    counter *c = a->c;
    for (long i = 0; i < n; i++)
    {
        increment(c);
    }
    return NULL;
}

struct timeval start_time, end_time;

int main(int argc, char const *argv[])
{
    assert(argc > 2);
    int threads = atoi(argv[1]);

    counter *c;
    c = malloc(sizeof(counter));
    init(c);
    pthread_t p[threads];

    myargs *arg;
    arg = malloc(sizeof(myargs));
    arg->c = c;
    arg->n = atoi(argv[2]);

    gettimeofday(&start_time, NULL);
    long startThreadsec = start_time.tv_sec;
    long startThreadusec = start_time.tv_usec;
    for (int i = 0; i < threads; i++)
    {
        assert(pthread_create(&p[i], NULL, worker, (void *)arg) == 0);
    }

    for (int i = 0; i < threads; i++)
    {
        assert(pthread_join(p[i], NULL) == 0);
    }
    gettimeofday(&end_time, NULL);
    long endThreadsec = end_time.tv_sec;
    long endThreadusec = end_time.tv_usec;

    gettimeofday(&start_time, NULL);
    long startWorkersec = start_time.tv_sec;
    long startWorkerusec = start_time.tv_usec;
    for (int i = 0; i < threads; i++)
    {
        worker((void *)arg);
    }
    gettimeofday(&end_time, NULL);
    long endWorkersec = end_time.tv_sec;
    long endWorkerusec = end_time.tv_usec;

    printf("Counter: %d\n\n", get(c));

    printf("Workersec: %lu \n", endWorkersec - startWorkersec);
    printf("Workerusec: %lu\n\n", endWorkerusec - startWorkerusec);

    printf("Threadsec: %lu \n", endThreadsec - startThreadsec);
    printf("Threadsec: %lu \n", endThreadusec - startThreadusec);

    free(arg);
    free(c);
    return 0;
}
