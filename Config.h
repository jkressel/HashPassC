//
// Created by alistair on 17/02/2021.
//

#ifndef HASHPASSC_CONFIG_H
#define HASHPASSC_CONFIG_H
#include <stdio.h>
#include <errno.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
typedef struct Config
{
    int first_time;
    char *db_encryption_hash;
    char *salt;
    char *encryption_pass;

}Config;

void init_config(Config* config);

void destroy_config(Config *config);

FILE* open_file_if_exists_and_check_if_first_time(Config* config);

void read_config_data(Config* config, FILE* fp);

void create_crypto_pass_hash(char* pass, Config* config);

int crypto_hashes_match(char* pass, Config* config);

void create_salt(Config* config);

#endif //HASHPASSC_CONFIG_H
