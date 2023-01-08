// https://www.unix.com/programming/81639-rdtsc-use-c.html

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

typedef unsigned long long ticks;

#define CLOCK_SPEED 3000000

static __inline__ ticks getticks(void)
{
    unsigned a, d;
    asm("cpuid");
    asm volatile("rdtsc"
                 : "=a"(a), "=d"(d));

    return (((ticks)a) | (((ticks)d) << 32));
}
int main()
{
    int r = 0;
    ticks tick, tick1, tickh;
    unsigned time = 0;

    tick = getticks();

    sleep(5);

    tick1 = getticks();

    time = (unsigned)((tick1 - tick) / CLOCK_SPEED);
    printf("\ntime :%u ms\n", time);
    return 0;
}