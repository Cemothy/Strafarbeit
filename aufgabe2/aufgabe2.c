#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

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

void arg_init(myargs *a, counter *c, long n)
{
    a->c = c;
    a->n = n;
}

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

int main(int argc, char *argv[])
{
    long n = 5;
    int threads = 4;
    int opt = 0;

    char *helpstring = "Options:\n-t threads\n-n n\n";

    while ((opt = getopt(argc, argv, "n:t:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            n = atoi(optarg);
            printf("ARG n = %ld\n", n);
            break;
        case 't':
            threads = atoi(optarg);
            printf("ARG threads = %d\n", threads);
            break;
        default:
            printf("%s", helpstring);
        }
    }

    counter *c;
    c = malloc(sizeof(counter));
    init(c);
    pthread_t p[threads];

    myargs *arg;
    arg = malloc(sizeof(myargs));
    arg_init(arg, c, n);

    gettimeofday(&start_time, NULL);

    for (int i = 0; i < threads; i++)
    {
        assert(pthread_create(&p[i], NULL, worker, (void *)arg) == 0);
    }

    for (int i = 0; i < threads; i++)
    {
        assert(pthread_join(p[i], NULL) == 0);
    }
    gettimeofday(&end_time, NULL);

    double time1 = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    time1 += (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    gettimeofday(&start_time, NULL);

    for (int i = 0; i < threads; i++)
    {
        worker((void *)arg);
    }
    gettimeofday(&end_time, NULL);

    printf("Counter: %d\n\n", get(c));

    double time2 = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    time2 += (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    printf("Time for %d Worker()-Threads parallel:             %lf\n", threads, time2);

    printf("Time for Executing Worker() %d times subsequently: %lf\n", threads, time1);

    free(arg);
    free(c);
    return 0;
}
