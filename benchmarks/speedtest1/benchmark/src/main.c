#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "speedtest1.h"

#define LAUNCH_TEST(test_num) \
    static struct timespec ts_start##test_num, ts_end##test_num; struct timespec ts_delta##test_num; \
    clock_gettime(CLOCK_MONOTONIC, &ts_start##test_num); \
    speedtest1_launch##test_num(); \
    clock_gettime(CLOCK_MONOTONIC, &ts_end##test_num); \
    timespec_diff(&ts_end##test_num, &ts_start##test_num, &ts_delta##test_num); \
    long long t##test_num = timespec_to_micro(ts_delta##test_num);

// Inspired from sys/time.h
# define timespec_diff(a, b, result)                  \
  do {                                                \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;     \
    (result)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec;  \
    if ((result)->tv_nsec < 0) {                      \
      --(result)->tv_sec;                             \
      (result)->tv_nsec += 1000000000;                \
    }                                                 \
  } while (0)

#define timespec_to_micro(t) \
    t.tv_sec * 1000 * 1000 + t.tv_nsec / 1000

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf ("Usage: [0(in-memory)|1(in-file)]\n");
        return 1;
    }

    int database_type = strtol(argv[1], NULL, 10);
    
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

    printf("%d, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld\n",
        database_type, t100, t110, t120, t130, t140, t142, t145, t160, t161, t170, t180, t190, t210,
        t230, t240, t250, t260, t270, t280, t290, t300, t320, t400, t410, t500, t510, t520, t980, t990);

    return 0;
}