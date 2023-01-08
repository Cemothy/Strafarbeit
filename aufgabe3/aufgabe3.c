#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct
{
    int num_threads;
    int threshold; // max value for local_counters counters

    int global_counter;
    int *local_counters; // count on their own till threshold is reached

    pthread_mutex_t *local_locks;
    pthread_mutex_t global_lock;
} sloppy_c_t;

void counter_init(sloppy_c_t *c, int num_threads, int threshold)
{
    c->num_threads = num_threads;
    c->threshold = threshold;
    c->global_counter = 0;

    c->local_counters = (int *)malloc(c->num_threads * sizeof(int));
    c->local_locks = (pthread_mutex_t *)malloc(c->num_threads * sizeof(pthread_mutex_t));
    if (c->local_counters == NULL)
    {
        fprintf(stderr, "counter_init: malloc local_counters failed\n");
    }
    else if (c->local_locks == NULL)
    {
        fprintf(stderr, "counter_init: malloc local_locks failed\n");
    }

    for (int i = 0; i < c->num_threads; i++)
    {
        assert(pthread_mutex_init((c->local_locks + i), NULL) == 0);
        *(c->local_counters + i) = 0;
    }
    assert(pthread_mutex_init(&c->global_lock, NULL) == 0);
}

void counter_delete(sloppy_c_t *c)
{
    for (int i = 0; i < c->num_threads; i++)
    {
        pthread_mutex_destroy(c->local_locks + i);
    }
    pthread_mutex_destroy(&c->global_lock);
    free(c->local_counters);
    free(c->local_locks);
    free(c);
}

typedef struct
{
    int inc_amnt;
    int thread_id;
    sloppy_c_t *c;
    int count_limit;
} arg_t;

void arg_init(arg_t *a, int inc_amt, sloppy_c_t *c, int num_threads, int count_limit)
{
    for (int i = 0; i < num_threads; i++)
    {
        (a + i)->count_limit = count_limit;
        (a + i)->inc_amnt = inc_amt;
        (a + i)->thread_id = i;
        (a + i)->c = c;
    }
}

void *update(void *args)
{
    arg_t *a = (arg_t *)args;
    int amnt = a->inc_amnt;
    int thread_id = a->thread_id;
    sloppy_c_t *c = a->c;
    assert(pthread_mutex_lock(&c->local_locks[thread_id]) == 0);
    for (int i = 0; i < a->count_limit / c->num_threads; i++)
    {
        c->local_counters[thread_id] += amnt;
        if (c->local_counters[thread_id] >= c->threshold)
        {
            assert(pthread_mutex_lock(&c->global_lock) == 0);
            c->global_counter += c->local_counters[thread_id];
            assert(pthread_mutex_unlock(&c->global_lock) == 0);
            c->local_counters[thread_id] = 0;
        }
    }
    assert(pthread_mutex_unlock(&c->local_locks[thread_id]) == 0);
    return NULL;
}

int main(int argc, char *argv[])
{
    int number_threads = 10;
    int number_cpus = sysconf(_SC_NPROCESSORS_CONF);
    int threshold = 5;
    int inc_amnt = 1;
    int count_limit = 1000;
    int print_to_file = 0;
    printf("ENV: number_cpus = %d\n", number_cpus);

    int opt = 0;
    char *helptext = "Options:\n-t number_threads\n-T threshold\n-h Help\n-l count_limit";
    while ((opt = getopt(argc, argv, "h:t:T:a:l:F:")) != -1)
    {
        switch (opt)
        {
        case 't':
            number_threads = atoi(optarg);
            printf("ARG number_threads = %d\n", number_threads);
            break;
        case 'T':
            threshold = atoi(optarg);
            printf("ARG threshold = %d\n", threshold);
            break;
        case 'a':
            inc_amnt = atoi(optarg);
            break;
        case 'l':
            printf("ARG threshold = %d\n", count_limit);
            break;
        case 'h':
            printf("%s", helptext);
            break;
        case 'F':
            print_to_file = atoi(optarg);
            // printf("ARG print_to_file = %d\n", print_to_file);
            if (print_to_file == 2)
            {
                FILE *file = fopen("out.csv", "a");
                assert(file != NULL);
                assert(0 < fprintf(file, "number_threads, threshold, inc_amnt, time\n"));
                return 0;
            }
            break;
        default:
            printf("%s", helptext);
            break;
        }
    }

    sloppy_c_t *c = malloc(sizeof(sloppy_c_t));
    counter_init(c, number_threads, threshold);

    arg_t *a = malloc(number_threads * sizeof(arg_t));
    arg_init(a, inc_amnt, c, number_threads, count_limit);

    pthread_t *threads = malloc(number_threads * sizeof(pthread_t));

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    for (int i = 0; i < number_threads; i++)
    {
        pthread_create((threads + i), NULL, update, (void *)(a + i));
    }

    for (int i = 0; i < number_threads; i++)
    {
        pthread_join(*(threads + i), NULL);
    }

    gettimeofday(&t1, NULL);

    double time = (t1.tv_sec - t0.tv_sec) * 1000.0;
    time += (t1.tv_usec - t0.tv_usec) / 1000.0;
    if (print_to_file == 0)
    {
        printf("\nElapsed Time: %lf ms\n", time);
    }
    else
    {
        FILE *file = fopen("out.csv", "a");
        assert(file != NULL);
        assert(0 < fprintf(file, "%d, %d, %d, %lf\n", number_threads, threshold, inc_amnt, time));
        assert(fclose(file) == 0);
    }

    counter_delete(c);
    free(threads);
    free(a);
    return 0;
}