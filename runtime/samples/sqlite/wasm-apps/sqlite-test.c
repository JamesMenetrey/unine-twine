#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../dist/sqlite/sqlite3.h"

#define DATABASE_FILENAME "db.db"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;

    for(i=0; i<argc; i++){
        fprintf(stderr, "%s = %s; ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    fprintf(stderr, "\n");
    return 0;
}

int main(int argc, char* argv[]){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    fprintf(stderr, "Starting SQLite %s in memory.\n", SQLITE_VERSION);

    struct stat s;
    int is_db_exists = stat(DATABASE_FILENAME, &s) == 0;

    rc = sqlite3_open(DATABASE_FILENAME, &db); // :memory:
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (!is_db_exists) {
        fprintf(stderr, "The database does not exist, creating one.\n");

        // Make sure the database is clear,
        // (required for WAMR, otherwise the internal number of pages of SQLite is not set to zero).
        fprintf(stderr, "Cleaning the database..\n");
        sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 1, 0);
        sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", 0, 0, 0);
        sqlite3_exec(db, "VACUUM", 0, 0, 0);
        sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 0, 0);

        fprintf(stderr, "Creating a table..\n");
        rc = sqlite3_exec(db, "CREATE TABLE person(firstname VARCHAR(10), lastname VARCHAR(10));", callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return -1;
        }
    } else {
        sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", 0, 0, 0);
    }

    fprintf(stderr, "Inserting data..\n");
    rc = sqlite3_exec(db, "INSERT INTO person VALUES('James', 'Menetrey');", callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    rc = sqlite3_exec(db, "INSERT INTO person VALUES('Pascal', 'Felber');", callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    fprintf(stderr, "Querying the table..\n");
    sqlite3_exec(db, "SELECT * FROM person;", callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    fprintf(stderr, "Querying the UTC time..\n");
    sqlite3_exec(db, "SELECT datetime('now','localtime') AS current_time;", callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    
    fprintf(stderr, "Closing SQLite.");
    sqlite3_close(db);
    return 0;
}