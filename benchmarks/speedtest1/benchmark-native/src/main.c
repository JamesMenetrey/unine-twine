#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "speedtest1.h"

#define LAUNCH_TEST(test_num) \
    clock_t t##test_num; \
    start = clock(); \
    speedtest1_launch##test_num(); \
    t##test_num = clock() - start;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf ("Usage: [0(in-memory)|1(in-file)]\n");
        return 1;
    }

    int database_type = strtol(argv[1], NULL, 10);
    
    clock_t start;
    
    speedtest1_initialize(database_type);

    LAUNCH_TEST(100)
    LAUNCH_TEST(110)
    LAUNCH_TEST(120)
    LAUNCH_TEST(130)
    LAUNCH_TEST(140)
    LAUNCH_TEST(142)
    LAUNCH_TEST(145)
    LAUNCH_TEST(160)
    LAUNCH_TEST(161)
    LAUNCH_TEST(170)
    LAUNCH_TEST(180)
    LAUNCH_TEST(190)
    LAUNCH_TEST(210)
    LAUNCH_TEST(230)
    LAUNCH_TEST(240)
    LAUNCH_TEST(250)
    LAUNCH_TEST(260)
    LAUNCH_TEST(270)
    LAUNCH_TEST(280)
    LAUNCH_TEST(290)
    LAUNCH_TEST(300)
    LAUNCH_TEST(320)
    LAUNCH_TEST(400)
    LAUNCH_TEST(410)
    LAUNCH_TEST(500)
    LAUNCH_TEST(510)
    LAUNCH_TEST(520)
    LAUNCH_TEST(980)
    LAUNCH_TEST(990)

    printf("%d, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n",
        database_type, t100, t110, t120, t130, t140, t142, t145, t160, t161, t170, t180, t190, t210,
        t230, t240, t250, t260, t270, t280, t290, t300, t320, t400, t410, t500, t510, t520, t980, t990);

    return 0;
}