#ifndef COMMON_H
#define COMMON_H

#include "benchmark_types.h"

void open_db(database_type_t database_type, int number_of_write);
void close_db(void);
void setup_database(void);
void init_rand(void);
void insert_data(int number_of_write);
void query_data_sequential(int number_of_data, int number_of_read);
void query_data_random(int number_of_data, int number_of_read);
void print_memory_usage(void);

#endif // COMMON_H