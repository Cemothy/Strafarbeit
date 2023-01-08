#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    long n = 100;

    if (argc > 1)
    {
        n = atol(argv[1]);
    }

    struct timeval current_time, end_time;
    gettimeofday(&current_time, NULL);

    for (long i = 0; i < n; i++)
        ;
    gettimeofday(&end_time, NULL);
    printf("Zeit(sec): %lu\n", end_time.tv_sec - current_time.tv_sec);
    printf("Zeit(usec): %lu\n", end_time.tv_usec - current_time.tv_usec);
    /* code */
    return 0;
}
