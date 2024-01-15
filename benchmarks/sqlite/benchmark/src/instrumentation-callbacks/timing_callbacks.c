#include <stdio.h>

#include "instrumentation_callbacks.h"

void on_initialization_finished() {}

void on_records_inserted(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
    printf("i,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, time);
}

void on_sequential_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
    printf("qs,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, time);
}

void on_random_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
    printf("qr,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, time);
}