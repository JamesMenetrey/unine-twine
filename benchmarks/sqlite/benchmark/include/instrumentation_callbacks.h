#ifndef DEBUG_CALLBACKS_H_
#define DEBUG_CALLBACKS_H_

#ifdef __cplusplus
extern "C" {
#endif

void on_initialization_finished();

void on_records_inserted(int database_type, int number_of_write,
                         int number_of_read, long int time);

void on_sequential_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time);

void on_random_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time);

#ifdef __cplusplus
}
#endif

#endif