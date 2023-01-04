// testopt.c

#include<stdio.h>
#include<unistd.h>

int main (int argc, char *argv[])
{
    if (argc > 1)
    {
        int c;

        while ((c = getopt(argc, argv, "oec:")) != -1)
        {
            printf("getopt(argc, argv, \"oec:\") != -1\n");
            printf("%c", (char) c);
        }
    }

    for (int i = 0; i < )
}