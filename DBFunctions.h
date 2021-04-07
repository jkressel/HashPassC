#ifndef DB_H   /* Include guard */
#define DB_H

#include "sqlite3.h"
#include "CryptoFunctions.h"
#include "Config.h"

typedef struct DB_RECORD
{
    unsigned char *username;
    int user_len;
    unsigned char *salt;
    int salt_len;
    unsigned char *notes;
    int notes_len;
    char *name;
    int length;
    char *allowed;
    unsigned char *crypto_salt;
    int uid;
}DB_RECORD;

typedef struct PASSWORD_DATA
{
    unsigned char *username;
    unsigned char *salt;
    unsigned char *notes;
    unsigned char *name;
    char *allowed;
    int length;
    int uid;
}PASSWORD_DATA;

typedef struct DB_RECORDS{
    DB_RECORD *db_records;
    int number_of_records;
}DB_RECORDS;

const int DB_READ_OK;

int open_db(char* filename, sqlite3 **db);

int setup_db(sqlite3 *db);

int close_db(sqlite3 *db);

int insert_into_db(sqlite3 *db, DB_RECORD *db_record);

int read_all_from_db(sqlite3 *db, DB_RECORDS *records);

void create_db_records(DB_RECORDS *db_records, int count);

void destroy_db_records(DB_RECORDS *db_records);

int delete_from_db(sqlite3 *db, int *uid);

void destroy_db_record(DB_RECORD *db_record);

int copy_db_record(DB_RECORD *source, DB_RECORD *dest);

int update_record(sqlite3 *db, DB_RECORD *update);

void setup_password_data(PASSWORD_DATA *password_data);

void destroy_password_data(PASSWORD_DATA *password_data);

void password_data_to_db_record(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config);

void db_record_to_password_data(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config);

#endif