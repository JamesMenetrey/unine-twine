#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "benchmark.h"
#include "instrumentation_callbacks.h"
#include "random.h"

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
    if (argc != 6)
    {
        printf ("Usage: [0(in-memory)|1(in-file)] number_of_write number_of_read print_memory_usage profiling_level\n");
        return 1;
    }

    database_type_t database_type = (database_type_t)strtol(argv[1], NULL, 10);
    int number_of_write = strtol(argv[2], NULL, 10);
    int number_of_read = strtol(argv[3], NULL, 10);
    int must_print_memory_usage = strtol(argv[4], NULL, 10);
    int profiling_level = strtol(argv[5], NULL, 10);
    struct timespec ts_start, ts_end, ts_delta;
    long long time_micro;

    if (profiling_level != 0)
    {
        printf("Profiling is not supported.\n");
        return 1;
    }

    on_initialization_finished();

    init_rand();
    open_db(database_type, number_of_write);
    setup_database();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
    insert_data(number_of_write);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
    timespec_diff(&ts_end, &ts_start, &ts_delta);
    time_micro = timespec_to_micro(ts_delta);
    on_records_inserted(database_type, number_of_write, number_of_read, time_micro);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
    query_data_sequential(number_of_write, number_of_read);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
    timespec_diff(&ts_end, &ts_start, &ts_delta);
    time_micro = timespec_to_micro(ts_delta);
    on_sequential_queries_finished(database_type, number_of_write, number_of_read, time_micro);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
    query_data_random(number_of_write, number_of_read);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
    timespec_diff(&ts_end, &ts_start, &ts_delta);
    time_micro = timespec_to_micro(ts_delta);
    on_random_queries_finished(database_type, number_of_write, number_of_read, time_micro);

    if (must_print_memory_usage) print_memory_usage();

    close_db();
}