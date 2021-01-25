#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "benchmark.h"
#include "instrumentation_callbacks.h"
#include "random.h"

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

    if (profiling_level != 0)
    {
        printf("Profiling is not supported.\n");
        return 1;
    }

    clock_t start, end;

    on_initialization_finished();

    init_rand();
    open_db(database_type, number_of_write);
    setup_database();

    start = clock();
    insert_data(number_of_write);
    end = clock();
    on_records_inserted(database_type, number_of_write, number_of_read, end - start);

    start = clock();
    query_data_sequential(number_of_write, number_of_read);
    end = clock();
    on_sequential_queries_finished(database_type, number_of_write, number_of_read, end - start);

    start = clock();
    query_data_random(number_of_write, number_of_read);
    end = clock();
    on_random_queries_finished(database_type, number_of_write, number_of_read, end - start);

    if (must_print_memory_usage) print_memory_usage();

    close_db();
}