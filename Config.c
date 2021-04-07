//
// Created by alistair on 17/02/2021.
//
#include <string.h>
#include <stdlib.h>
#include "Config.h"

/**
 * allocates memory for the config
 * @param config
 */
void init_config(Config* config) {
    config->first_time = 0;
    config->db_encryption_hash = malloc(SHA512_DIGEST_LENGTH * sizeof(char));
    config->salt = malloc(SHA512_DIGEST_LENGTH * sizeof(char));
    config->encryption_pass = malloc(50 * sizeof(char));
}

/**
 * frees config
 * @param config
 */
void destroy_config(Config *config) {
    free(config->db_encryption_hash);
    free(config->salt);
    free(config->encryption_pass);
}

/**
 * open the config file and check if this is the first run in doing so
 * @param config
 * @return file pointer to the config file
 */
FILE* open_file_if_exists_and_check_if_first_time(Config* config) {
    FILE *fp = fopen("hashpassconfig.txt", "r+");
    if (fp == NULL) {
        if (errno == ENOENT) {
            fp = fopen("hashpassconfig.txt", "w");
            config->first_time = 1;
        } else
            printf("%s", "error opening the file");
    } else {
        config->first_time = 0;
    }
    return fp;
}

/**
 * read data from the config file
 * @param config
 * @param fp
 */
void read_config_data(Config* config, FILE* fp) {
    char line[100];

    fgets(line, sizeof(line), fp);
    strncpy(config->db_encryption_hash, line, SHA512_DIGEST_LENGTH);
    fgets(line, sizeof(line), fp);
    strncpy(config->salt, line, SHA512_DIGEST_LENGTH);


}

/**
 * calculate the absolute of a given value
 * @param value
 * @return
 */
int calculate_abs(int value) {
    return abs(value);
}

/**
 * create crypto salt
 * @param config
 */
void create_salt(Config *config) {
    RAND_bytes(config->salt, SHA512_DIGEST_LENGTH);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        config->salt[i] = (char)(calculate_abs(config->salt[i])%93) + 33;
}

/**
 * create hash of encryption password to verify that the user has entered the correct one
 * @param pass
 * @param config
 */
void create_crypto_pass_hash(char* pass, Config* config) {
    unsigned char *hash = malloc(SHA512_DIGEST_LENGTH * sizeof(char));
    char *wholePass = malloc(SHA512_DIGEST_LENGTH * 2 * sizeof(char));
    strncpy(wholePass, pass, SHA512_DIGEST_LENGTH);
    strncat(wholePass, config->salt, SHA512_DIGEST_LENGTH);
    SHA512(wholePass, strlen(wholePass), hash);
    strncpy(config->db_encryption_hash, hash, SHA512_DIGEST_LENGTH);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        config->db_encryption_hash[i] = (char) (calculate_abs(config->db_encryption_hash[i]) % 93) + 33;
    }
    free(hash);
    free(wholePass);
}

/**
 * check that the stored hash and user generated hashes match, indicating that the passwords match
 * @param pass
 * @param config
 * @return
 */
int crypto_hashes_match(char* pass, Config* config) {
    char *hash = malloc(SHA512_DIGEST_LENGTH * sizeof(char));
    char *wholePass = malloc(SHA512_DIGEST_LENGTH * 2 * sizeof(char));
    strncpy(wholePass, pass, SHA512_DIGEST_LENGTH);
    strncat(wholePass, config->salt, SHA512_DIGEST_LENGTH);
    SHA512(wholePass, strlen(wholePass), hash);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        hash[i] = (char) (calculate_abs(hash[i]) % 93) + 33;
    }
    int cmp = strncmp(config->db_encryption_hash, hash, SHA512_DIGEST_LENGTH);
    free(hash);
    free(wholePass);
    return cmp;
}
