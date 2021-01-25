#include <stdio.h>
#include <stdlib.h>

#include "instrumentation_callbacks.h"

void on_initialization_finished() {}

void on_records_inserted(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
	fprintf(stdout, "Insertion done. Waiting for memory assessment.\n");
	getchar();
}

void on_sequential_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}

void on_random_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}