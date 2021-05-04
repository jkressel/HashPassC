//
// Created by alistair on 20/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DBFunctions.h"
#include "Base64Encode.h"
#include "Base64Decode.h"

const int DB_READ_OK = 0;


int prepare_stmt(sqlite3_stmt **res, sqlite3 *db, char* stmt) {
    const char *tail;
    return sqlite3_prepare_v2(db, stmt, -1, res, &tail);
}


/**
 * Opens an SQLITE3 database
 *
 * @param filename
 * @param db
 * @return returns sqlite3 status
 */
int open_db(char* filename, sqlite3 **db) {
    return sqlite3_open(filename, db);
}


/**
 * close sqlite3 db
 *
 * @param db
 * @return returns sqlite3 status
 */
int close_db(sqlite3 *db) {
    return sqlite3_close_v2(db);
}

/**
 * initialise the database
 *
 * @param db
 * @return sqlite3 status
 */
int setup_db(sqlite3 *db) {
    char stmt[] = "CREATE TABLE IF NOT EXISTS passwords(uid INTEGER PRIMARY KEY, salt BLOB NOT NULL, saltLen INT NOT NULL, username BLOB NOT NULL, usernameLen INT NOT NULL, notes BLOB, notesLen INT, name VARCHAR(300) NOT NULL, length INT NOT NULL, allowedChars VARCHAR(300) NOT NULL, cryptoSalt BLOB, cryptoSaltLength INT NOT NULL);";
    return sqlite3_exec(db, stmt, NULL, NULL, NULL);

}


/**
 * insert a DB_RECORD into the database
 * @param db
 * @param db_record
 * @return sqlite3 status
 */
int insert_into_db(sqlite3 *db, DB_RECORD *db_record) {
    char *query = sqlite3_mprintf("insert into passwords (salt, saltLen, username, usernameLen, notes, notesLen, name, length, allowedChars, cryptoSalt, cryptoSaltLength) values (%Q, %i, %Q, %i, %Q, %i, %Q, %i, %Q, %Q, %i);", db_record->salt, db_record->salt_len, db_record->username, db_record->user_len, db_record->notes, db_record->notes_len, db_record->name, db_record->length, db_record->allowed, db_record->crypto_salt, db_record->crypto_salt_len);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

/**
 * delete specified uid from the database
 * @param db
 * @param uid
 * @return sqlite3 status
 */
int delete_from_db(sqlite3 *db, int *uid) {
    char *query = sqlite3_mprintf("delete from passwords where uid=%i", *uid);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

/**
 * gets the number of records presently stored in the database
 * @param db
 * @return sqlite3 status
 */
int get_count(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int count = 0;
    sqlite3_prepare_v2(db, "select count(*) from passwords;", -1, &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}

/**
 * allocates the requited memory for DB_RECORD struct
 * @param db_record
 * @param salt_size
 * @param username_size
 * @param notes_size
 * @param crypto_salt_size
 */
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

/**
 * properly frees a DB_RECORD struct
 * @param db_record
 */
void destroy_db_record(DB_RECORD *db_record) {
    free(db_record->salt);
    free(db_record->username);
    free(db_record->name);
    free(db_record->notes);
    free(db_record->crypto_salt);
    free(db_record->allowed);
}

/**
 * allocates memory for DB_RECORDS struct - note that DB_RECORDS is essentially a list of DB_RECORD
 * @param db_records
 * @param count
 */
void create_db_records(DB_RECORDS *db_records, int count) {
    db_records->db_records = (struct DB_RECORD*)malloc(count*sizeof(struct DB_RECORD));
    db_records->number_of_records = count;
}

/**
 * frees DB_RECORDS
 * @param dbRecords
 */
void destroy_db_records(DB_RECORDS *dbRecords) {
    for (int i = 0; i < dbRecords->number_of_records; i++) {
        destroy_db_record(&dbRecords->db_records[i]);
    }
    free(dbRecords->db_records);
}

/**
 * read all records from the db and create a DB_RECORD for each
 * @param db
 * @param records
 * @return sqlite3 status
 */
int read_all_from_db(sqlite3 *db, DB_RECORDS *records) {
    sqlite3_stmt *stmt;
    int count = get_count(db);
    if (count > 0) {
        create_db_records(records, count);
        sqlite3_prepare_v2(db, "select * from passwords;", -1, &stmt, NULL);
        int i = 0;
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            create_db_record(&records->db_records[i], sqlite3_column_bytes(stmt, 1) + 1, sqlite3_column_bytes(stmt, 2) + 1, sqlite3_column_bytes(stmt, 3) + 1, sqlite3_column_bytes(stmt, 7) + 1);
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
            records->db_records[i].crypto_salt_len = sqlite3_column_int(stmt, 11);
            i++;

        }
        sqlite3_finalize(stmt);
    }

    return 0;


}

/**
 * update a database record
 * @param db
 * @param update
 * @return sqlite3 status
 */
int update_record(sqlite3 *db, DB_RECORD *update) {
    char *query = sqlite3_mprintf("update passwords set salt = %Q, username = %Q, usernameLen = %i, notes = %Q, notesLen = %i, name = %Q, length = %i, allowedChars = %Q, cryptoSalt = %Q, cryptoSaltLength = %i where uid = %i", update->salt, update->username, update->user_len, update->notes, update->notes_len, update->name, update->length, update->allowed, update->crypto_salt, update->crypto_salt_len, update->uid);
    int status = sqlite3_exec(db, query, NULL, NULL, NULL);
    if (status != SQLITE_OK) {
        printf("\n%s", sqlite3_errmsg(db));
    }
    sqlite3_free(query);
    return status;
}

/**
 * allocate memory for PASSWORD_DATA
 * @param password_data
 */
void create_password_data(PASSWORD_DATA *password_data) {
    password_data->allowed = (char *)malloc(301 * sizeof(char));
    password_data->name = (unsigned char *)malloc(301 * sizeof(char));
    password_data->notes = (unsigned char *)malloc(301 * sizeof(char));
    password_data->username = (unsigned char *)malloc(301 * sizeof(char));
    password_data->salt = (unsigned char *)malloc(65 * sizeof(char));
}

/**
 * free PASSWORD_DATA
 * @param password_data
 */
void destroy_password_data(PASSWORD_DATA *password_data) {
    free(password_data->allowed);
    free(password_data->name);
    free(password_data->notes);
    free(password_data->username);
    free(password_data->salt);
}

/**
 * create a copy of PASSWORD_DATA
 * @param password_data
 * @param copy_password_data
 */
void copy_password_data(PASSWORD_DATA *password_data, PASSWORD_DATA *copy_password_data) {
    strncpy(copy_password_data->name, password_data->name, strlen(password_data->name) + 1);
    strncpy(copy_password_data->username, password_data->username, strlen(password_data->username) + 1);
    strncpy(copy_password_data->notes, password_data->notes, strlen(password_data->notes) + 1);
    strncpy(copy_password_data->salt, password_data->salt, strlen(password_data->salt) + 1);
    strncpy(copy_password_data->allowed, password_data->allowed, strlen(password_data->allowed) + 1);

    copy_password_data->uid = password_data->uid;
    copy_password_data->length = password_data->length;
}

/**
 * convert PASSWORD_DATA to DB_RECORD ready to insert into the db, the involves encrypting the salt, username and notes
 * @param password_data
 * @param db_record
 * @param config
 */
void password_data_to_db_record(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config) {
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
    unsigned char *salt_val = generate_crypto_salt();

    char *enc_crypto;
    size_t enc_len;
    Base64Encode(salt_val, 8, &enc_crypto, &enc_len);
    enc_crypto[enc_len] = '\0';


    if (aes_init(config->encryption_pass, strlen(config->encryption_pass), enc_crypto, en, de)) {
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

    char *enc_user;
    char *enc_salt;
    char *enc_notes;

    Base64Encode(euser_name, username_len, &enc_user, &enc_len);
    enc_user[enc_len] = '\0';
    Base64Encode(esalt, e_salt_len, &enc_salt, &enc_len);
    enc_salt[enc_len] = '\0';
    Base64Encode(enotes, notes_txt_len, &enc_notes, &enc_len);
    enc_notes[enc_len] = '\0';

    create_db_record(db_record, strlen(enc_salt) + 1, strlen(enc_user) + 1, strlen(enc_notes) + 1, strlen(enc_crypto) + 1);

    int name_len = strlen(password_data->name);
    strncpy(db_record->name, password_data->name, name_len + 1);
    strncpy(db_record->allowed, password_data->allowed, strlen(password_data->allowed) + 1);
    strncpy(db_record->username, enc_user, strlen(enc_user) + 1);
    strncpy(db_record->salt, enc_salt, strlen(enc_salt) + 1);
    strncpy(db_record->notes, enc_notes, strlen(enc_notes) + 1);
    strncpy(db_record->crypto_salt, enc_crypto, strlen(enc_crypto) + 1);
    db_record->length = password_data->length;
    db_record->notes_len = strlen(db_record->notes);
    db_record->salt_len = strlen(db_record->salt);
    db_record->user_len = strlen(db_record->username);
    db_record->uid = password_data->uid;
    db_record->crypto_salt_len = strlen(enc_crypto);

    endpdtodbr: EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);
    free(esalt);
    free(euser_name);
    free(enotes);
    free(salt_val);
    free(enc_notes);
    free(enc_user);
    free(enc_salt);
    free(enc_crypto);

}

/**
 * convert DB_RECORD to PASSWORD_DATA, this involves decrypting the salt, username and notes
 * @param password_data
 * @param db_record
 * @param config
 */
void db_record_to_password_data(PASSWORD_DATA *password_data, DB_RECORD *db_record, Config *config) {



    unsigned char *dsalt;
    unsigned char *duser_name;
    unsigned char *dnotes;

    //do the decryption
    char *dec_salt;
    char *dec_user;
    char *dec_notes;
    db_record->salt[db_record->salt_len] = '\0';
    db_record->username[db_record->user_len] = '\0';
    db_record->notes[db_record->notes_len] = '\0';
    db_record->crypto_salt[db_record->crypto_salt_len] = '\0';
    printf("%s\n", db_record->salt);
    printf("%s\n", db_record->username);
    printf("%s\n", db_record->notes);
    printf("%s\n", db_record->crypto_salt);
    Base64Decode(db_record->salt, &dec_salt, &db_record->salt_len);
    Base64Decode(db_record->username, &dec_user, &db_record->user_len);
    Base64Decode(db_record->notes, &dec_notes, &db_record->notes_len);
    printf("%d\n", db_record->salt_len);
    printf("%d\n", db_record->user_len);
    printf("%d\n", db_record->notes_len);

    //cipher ctx
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();

    printf("%s\n", config->encryption_pass);
    //let's check if we can even initialise aes
    if (aes_init(config->encryption_pass, strlen(config->encryption_pass), db_record->crypto_salt, en, de)) {
        printf("Couldn't initialize AES cipher\n");
        goto enddbrtopr;
    }

    dsalt = decrypt(de, dec_salt, &db_record->salt_len);
    duser_name = decrypt(de, dec_user, &db_record->user_len);
    dnotes = decrypt(de, dec_notes, &db_record->notes_len);

    free(dec_salt);
    free(dec_user);
    free(dec_notes);


    strncpy(password_data->name, db_record->name, strlen(db_record->name) + 1);
    password_data->name[strlen(db_record->name)] = '\0';
    strncpy(password_data->allowed, db_record->allowed, strlen(db_record->allowed));
    password_data->allowed[strlen(db_record->allowed)] = '\0';
    strncpy(password_data->username, duser_name, db_record->user_len);
    password_data->username[db_record->user_len] = '\0';
    strncpy(password_data->salt, dsalt, 65);
    password_data->salt[64] = '\0';
    strncpy(password_data->notes, dnotes, db_record->notes_len);
    password_data->notes[db_record->notes_len] = '\0';
    password_data->length = db_record->length;
    password_data->uid = db_record->uid;


    //free, we don't need them anymore
    enddbrtopr: free(dsalt);
    free(duser_name);
    free(dnotes);

    EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);

}

