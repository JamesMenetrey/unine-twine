#include <stdio.h>
#include <stdlib.h>

#include "instrumentation_callbacks.h"

void on_initialization_finished()
{
	fprintf(stdout, "Initialization done. Waiting for time measurement.\n");
	getchar();
    exit(0);
}

void on_records_inserted(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}

void on_sequential_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}

void on_random_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}