#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include "benchmark.h"
#include "random.h"

// Number of pages.
// Default cache size = FLAGS_page_size * FLAGS_num_pages = 8 MB.
static int FLAGS_num_pages = 2048;

// Page size. Default 4 KB.
static int FLAGS_page_size = 4096;

// Journal mode. Default DELETE mode.
static char* FLAGS_journal_mode = "DELETE";

// Synchronization mode. Default NORMAL mode.
static char* FLAGS_synchronization_mode = "NORMAL";

static sqlite3* db;

static void* database_memory = NULL;

inline static void error_check_with_msg(int status, char* err_msg)
{
    if (status != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s (status %d)\n", err_msg, status);
        sqlite3_free(err_msg);
    }
}

inline static void error_check(int status)
{
    if (status != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: status: %d\n", status);
    }
}

inline static void execute(const char* query)
{
    char* err_msg = NULL;
    int status;
    
    status = sqlite3_exec(db, query, NULL, NULL, &err_msg);
    error_check_with_msg(status, err_msg);
}

inline static int compute_overhead_for_records(int number_of_write)
{
    // For 1k records, we measured a minimum overhead of 1M of memory, while
    // for 1M records, we measured a minimum overhead of 15M of memory.
    // The following coefficients are found by assuming a linear relation exists
    // between these two points. Hence we find the related linear equation.
    int a = 81, b = 919541;

    return a * number_of_write + b;
}

void open_db(database_type_t database_type, int number_of_write)
{
    // Configure SQLite to use a big buffer of memory for its heap
    int size = 1024 * number_of_write + compute_overhead_for_records(number_of_write);
    database_memory = malloc(size);
    memset(database_memory, 0, size);

    int status = sqlite3_config(SQLITE_CONFIG_HEAP, database_memory, size, 2);
    if (status)
    {
        fprintf(stderr, "Can't configure the heap buffer of SQLite. status: %d\n", status);
        return;
    }

    // Open the database
    char query[100];
    char* fileName = database_type == In_Memory
        ? ":memory:"
        : "benchmark.db";
    
    status = sqlite3_open(fileName, &db);
    if (status)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // Make sure the database is clear and the settings applied,
    // (required for WAMR, otherwise the internal number of pages of SQLite is not set to zero).
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 1, 0);

    snprintf(query, 100, "PRAGMA cache_size = %d", FLAGS_num_pages);
    execute(query);

    snprintf(query, 100, "PRAGMA page_size = %d", FLAGS_page_size);
    execute(query);

    snprintf(query, 100, "PRAGMA synchronous = %s", FLAGS_synchronization_mode);
    execute(query);

    sqlite3_exec(db, "VACUUM", 0, 0, 0);
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 0, 0);
}

void close_db(void)
{
    int status = sqlite3_close(db);
    if (status != SQLITE_OK)
    {
        fprintf(stderr, "Can't close database: %d\n", status);
    }

    free(database_memory);
}

void setup_database(void)
{
    char query[100];

    execute("CREATE TABLE Benchmark(Id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, Body BLOB);");
}

void init_rand(void)
{
    prng_init();
}

void insert_data(int number_of_write)
{
    char* rand_buffer;
    int status;
    sqlite3_stmt *stmt = NULL;

    execute("BEGIN TRANSACTION");

    status = sqlite3_prepare_v2(db, "INSERT INTO Benchmark (Body) VALUES(?);", -1, &stmt, NULL);
    error_check(status);

    int i;
    for (i = 0; i < number_of_write; i++)
    {
        status = sqlite3_bind_blob(stmt, 1, prng_get_cached_slice(1000), 1000, SQLITE_STATIC);
        error_check(status);

        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE)
        {
            fprintf(stderr, "SQL error (step): status: %d. Occurred after %d insertion.\n", status, i);
            return;
        }

        status = sqlite3_clear_bindings(stmt);
        error_check(status);

        sqlite3_reset(stmt);
        error_check(status);
    }

    
    status = sqlite3_finalize(stmt);
    error_check(status);

    execute("END TRANSACTION");
}

void query_data_sequential(int number_of_data, int number_of_read)
{
    unsigned int* rand_buffer;
    int status;
    sqlite3_stmt *stmt = NULL;

    execute("BEGIN TRANSACTION");
    
    status = sqlite3_prepare_v2(db, "SELECT * FROM Benchmark WHERE Id = ?", -1, &stmt, NULL);
    error_check(status);

    int i;
    for (i = 0; i < number_of_data; i++)
    {
        status = sqlite3_bind_int(stmt, 1, (i % number_of_data) + 1);
        error_check(status);

        status = sqlite3_step(stmt);
        if (status != SQLITE_ROW)
        {
            fprintf(stderr, "SQL error (step): status: %d\n", status);
            return;
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    
    execute("END TRANSACTION");
}

void query_data_random(int number_of_data, int number_of_read)
{
    unsigned int* rand_buffer;
    int status;
    sqlite3_stmt *stmt = NULL;

    execute("BEGIN TRANSACTION");
    
    status = sqlite3_prepare_v2(db, "SELECT * FROM Benchmark WHERE Id = ?", -1, &stmt, NULL);
    error_check(status);

    int i;
    for (i = 0; i < number_of_data; i++)
    {
        status = sqlite3_bind_int(stmt, 1, prng_get_next_nonuniform_between(1, number_of_data)); // Range of [1; number_of_data-1[ because AUTOINCREMENT starts with 1.
        error_check(status);

        status = sqlite3_step(stmt);
        if (status != SQLITE_ROW)
        {
            fprintf(stderr, "SQL error (step): status: %d\n", status);
            return;
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    
    execute("END TRANSACTION");
}

void print_memory_usage(void)
{
    int value, highestValue;

    sqlite3_status(SQLITE_STATUS_MEMORY_USED, &value, &highestValue, 0);
    fprintf(stderr, "[SQLITE_STATUS_MEMORY_USED] value: %d (%.2f MB), highestValue: %d (%.2f MB)\n",
        value, (float)value / 1024 / 1024, highestValue, (float)highestValue / 1024 / 1024);

    sqlite3_status(SQLITE_STATUS_PAGECACHE_USED, &value, &highestValue, 0);
    fprintf(stderr, "[SQLITE_STATUS_PAGECACHE_USED] value: %d (%.2f MB), highestValue: %d (%.2f MB)\n",
        value, (float)value / 1024 / 1024, highestValue, (float)highestValue / 1024 / 1024);

    sqlite3_db_status(db, SQLITE_DBSTATUS_LOOKASIDE_USED, &value, &highestValue, 0);
    fprintf(stderr, "[SQLITE_DBSTATUS_LOOKASIDE_USED] value: %d slots, highestValue: %d slots\n",
        value, highestValue);

    sqlite3_db_status(db, SQLITE_DBSTATUS_CACHE_USED, &value, &highestValue, 0);
    fprintf(stderr, "[SQLITE_DBSTATUS_CACHE_USED] value: %d (%.2f MB), highestValue: %d (%.2f MB)\n",
        value, (float)value / 1024 / 1024, highestValue, (float)highestValue / 1024 / 1024);

    sqlite3_db_status(db, SQLITE_DBSTATUS_STMT_USED, &value, &highestValue, 0);
    fprintf(stderr, "[SQLITE_DBSTATUS_STMT_USED] value: %d (%.2f MB), highestValue: %d (%.2f MB)\n",
        value, (float)value / 1024 / 1024, highestValue, (float)highestValue / 1024 / 1024);
}

static int display_max_id_callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void display_max_id()
{
    int status;
    char *zErrMsg = 0;
    
    status = sqlite3_exec(db, "SELECT MAX(Id) FROM Benchmark", display_max_id_callback, 0, &zErrMsg);
    if (status != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    }
}