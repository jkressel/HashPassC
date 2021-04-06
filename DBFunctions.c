//
// Created by alistair on 20/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DBFunctions.h"

const int DB_READ_OK = 0;


int prepare_stmt(sqlite3_stmt **res, sqlite3 *db, char* stmt) {
    const char *tail;
    return sqlite3_prepare_v2(db, stmt, -1, res, &tail);
}

int open_db(char* filename, sqlite3 **db) {
    return sqlite3_open(filename, db);
}

int close_db(sqlite3 *db) {
    return sqlite3_close_v2(db);
}

int setup_db(sqlite3 *db) {
    char stmt[] = "CREATE TABLE IF NOT EXISTS passwords(uid INTEGER PRIMARY KEY, salt BLOB NOT NULL, saltLen INT NOT NULL, username BLOB NOT NULL, usernameLen INT NOT NULL, notes BLOB, notesLen INT, name VARCHAR(300) NOT NULL, length INT NOT NULL, allowedChars VARCHAR(300) NOT NULL, cryptoSalt BLOB);";
    return sqlite3_exec(db, stmt, NULL, NULL, NULL);

}

int insert_into_db(sqlite3 *db, DB_RECORD *db_record) {
    char *query = sqlite3_mprintf("insert into passwords (salt, saltLen, username, usernameLen, notes, notesLen, name, length, allowedChars, cryptoSalt) values (%Q, %i, %Q, %i, %Q, %i, %Q, %i, %Q, %Q);", db_record->salt, db_record->salt_len, db_record->username, db_record->user_len, db_record->notes, db_record->notes_len, db_record->name, db_record->length, db_record->allowed, db_record->crypto_salt);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

int delete_from_db(sqlite3 *db, int *uid) {
    char *query = sqlite3_mprintf("delete from passwords where uid=%i", *uid);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

int get_count(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int count = 0;
    sqlite3_prepare_v2(db, "select count(*) from passwords;", -1, &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}

void create_db_record(DB_RECORD *db_record, int salt_size, int username_size, int notes_size, int crypto_salt_size) {
    db_record->salt = malloc(salt_size * sizeof(char));
    db_record->username = malloc(username_size * sizeof(char));
    db_record->notes = malloc(notes_size * sizeof(char));
    db_record->crypto_salt = malloc(crypto_salt_size * sizeof(char));
    db_record->name = malloc (301 * sizeof(char));
    db_record->allowed = malloc(301 * sizeof(char));

    db_record->length = 0;
    db_record->uid = 0;
    db_record->notes_len = 0;
    db_record->user_len = 0;
    db_record->salt_len = 0;
}

void destroy_db_record(DB_RECORD *db_record) {
    free(db_record->salt);
    free(db_record->username);
    free(db_record->name);
    free(db_record->notes);
    free(db_record->crypto_salt);
    free(db_record->allowed);
}

void create_db_records(DB_RECORDS *db_records, int count) {
    db_records->db_records = (struct DB_RECORD*)malloc(count*sizeof(struct DB_RECORD));
    db_records->number_of_records = count;
}

void destroy_db_records(DB_RECORDS *dbRecords) {
    for (int i = 0; i < dbRecords->number_of_records; i++) {
        destroy_db_record(&dbRecords->db_records[i]);
    }
    free(dbRecords->db_records);
}


int read_all_from_db(sqlite3 *db, DB_RECORDS *records) {
    sqlite3_stmt *stmt;
    int count = get_count(db);
    if (count > 0) {
        create_db_records(records, count);
        sqlite3_prepare_v2(db, "select * from passwords;", -1, &stmt, NULL);
        int i = 0;
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            create_db_record(&records->db_records[i], sqlite3_column_bytes(stmt, 1), sqlite3_column_bytes(stmt, 2), sqlite3_column_bytes(stmt, 3), sqlite3_column_bytes(stmt, 7));
            records->db_records[i].uid = sqlite3_column_int(stmt, 0);
            memcpy(records->db_records[i].salt, sqlite3_column_text(stmt, 1), sqlite3_column_bytes(stmt, 1));
            records->db_records[i].salt_len = sqlite3_column_int(stmt, 2);
            memcpy(records->db_records[i].username, sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt, 3));
            records->db_records[i].user_len = sqlite3_column_int(stmt, 4);
            memcpy(records->db_records[i].notes, sqlite3_column_text(stmt, 5), sqlite3_column_bytes(stmt, 5));
            records->db_records[i].notes_len = sqlite3_column_int(stmt, 6);
            (&records->db_records[i])->name = strdup((const char*)sqlite3_column_text(stmt, 7));
            records->db_records[i].length = sqlite3_column_int(stmt, 8);
            records->db_records[i].allowed = strdup((const char*)sqlite3_column_text(stmt, 9));
            memcpy(records->db_records[i].crypto_salt, sqlite3_column_text(stmt, 10), sqlite3_column_bytes(stmt, 10));
            i++;

        }
        sqlite3_finalize(stmt);
    }

    return 0;


}

int copy_db_record(DB_RECORD *source, DB_RECORD *dest) {
    create_db_record(dest, sizeof(source->salt), sizeof(source->username), sizeof(source->notes), sizeof(source->crypto_salt));
    memcpy(dest->username, source->username, sizeof(source->username));
    memcpy(dest->salt, source->salt, sizeof(source->salt));
    memcpy(dest->notes, source->notes, sizeof(source->notes));
    memcpy(dest->crypto_salt, source->crypto_salt, sizeof(source->crypto_salt));
    strncpy(dest->name, source->name, strlen(source->name));
    strncpy(dest->allowed, source->allowed, strlen(source->allowed));
    dest->uid = source->uid;
    dest->user_len = source->user_len;
    dest->notes_len = source->notes_len;
    dest->length = source->length;
    return 0;

}

int update_record(sqlite3 *db, DB_RECORD *update) {
    char *query = sqlite3_mprintf("update passwords set salt = %Q, username = %Q, usernameLen = %i, notes = %Q, notesLen = %i, name = %Q, length = %i, allowedChars = %Q, cryptoSalt = %Q where uid = %i", update->salt, update->username, update->user_len, update->notes, update->notes_len, update->name, update->length, update->allowed, update->crypto_salt, update->uid);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

void setup_password_data(PASSWORD_DATA *password_data) {
    password_data->allowed = (char *)malloc(301 * sizeof(char));
    password_data->name = (unsigned char *)malloc(301 * sizeof(char));
    password_data->notes = (unsigned char *)malloc(301 * sizeof(char));
    password_data->username = (unsigned char *)malloc(301 * sizeof(char));
    password_data->salt = (unsigned char *)malloc(65 * sizeof(char));
}

void destroy_password_data(PASSWORD_DATA *password_data) {
    free(password_data->allowed);
    free(password_data->name);
    free(password_data->notes);
    free(password_data->username);
    free(password_data->salt);
}

void password_data_to_db_record(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config) {
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
    unsigned char *salt_val = generate_crypto_salt();


    if (aes_init(config->encryption_pass, strlen(config->encryption_pass), salt_val, en, de)) {
        printf("Couldn't initialize AES cipher\n");
        goto endpdtodbr;
    }
    unsigned char *esalt;
    unsigned char *euser_name;
    unsigned char *enotes;
    int e_salt_len = 65;
    int username_len = strlen(password_data->username) + 1;
    int notes_txt_len = strlen(password_data->notes) + 1;
    printf("%s", "Encryption started\n");
    esalt = encrypt(en, password_data->salt, &e_salt_len);
    euser_name = encrypt(en, password_data->username, &username_len);
    enotes = encrypt(en, password_data->notes, &notes_txt_len);
    printf("%s", "All encryption done\n");

    create_db_record(db_record, e_salt_len, username_len, notes_txt_len, 8);

    int name_len = strlen(password_data->name);
    strncpy(db_record->name, password_data->name, name_len + 1);
    strncpy(db_record->allowed, password_data->allowed, strlen(password_data->allowed) + 1);
    printf("\n%s\n%d", db_record->allowed, strlen(password_data->allowed));
    strncpy(db_record->username, euser_name, username_len);
    strncpy(db_record->salt, esalt, e_salt_len);
    strncpy(db_record->notes, enotes, notes_txt_len);
    strncpy(db_record->crypto_salt, salt_val, strlen(salt_val));
    db_record->length = password_data->length;
    db_record->notes_len = notes_txt_len;
    db_record->salt_len = e_salt_len;
    db_record->user_len = username_len;
    printf("%s\n", db_record->salt);
    printf("%s\n", db_record->username);
    printf("%s\n", db_record->notes);

    endpdtodbr: EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);
    free(esalt);
    free(euser_name);
    free(enotes);
    free(salt_val);

}

void db_record_to_password_data(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config) {



    unsigned char *dsalt;
    unsigned char *duser_name;
    unsigned char *dnotes;

    //cipher ctx
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();

    //let's check if we can even initialise aes
    if (aes_init(config->encryption_pass, strlen(config->encryption_pass), db_record->crypto_salt, en, de)) {
        printf("Couldn't initialize AES cipher\n");
        goto enddbrtopr;
    }

    printf("%s\n", db_record->salt);
    printf("%s\n", db_record->username);
    printf("%s\n", db_record->notes);


    //do the decryption
    dsalt = decrypt(de, db_record->salt, &db_record->salt_len);
    duser_name = decrypt(de, db_record->username, &db_record->user_len);
    dnotes = decrypt(de, db_record->notes, &db_record->notes_len);


//    strncpy(username, duser_name, db_record->user_len);
//    username[db_record->user_len] = '\0';
//    strncpy(notes, dnotes, db_record->notes_len);
//    notes[db_record->notes_len] = '\0';

    strncpy(password_data->name, db_record->name, strlen(db_record->name) + 1);
    password_data->name[strlen(db_record->name)] = '\0';
    strncpy(password_data->allowed, db_record->allowed, strlen(db_record->allowed));
    password_data->allowed[strlen(db_record->allowed)] = '\0';
    strncpy(password_data->username, duser_name, db_record->user_len);
    password_data->username[db_record->user_len] = '\0';
    printf("%s", duser_name);
    printf("%s", dnotes);
    strncpy(password_data->salt, dsalt, 65);
    password_data->salt[64] = '\0';
    printf("%s", password_data->salt);
    strncpy(password_data->notes, dnotes, db_record->notes_len);
    password_data->notes[db_record->notes_len] = '\0';
    password_data->length = db_record->length;


    //free, we don't need them anymore
    enddbrtopr: free(dsalt);
    free(duser_name);
    free(dnotes);

    EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);

}

