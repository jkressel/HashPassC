#include "PasswordFunctions.h"
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

char* get_password(CustomCharacters* cc, int pwd_length, char* full_pwd) {
    char hash[SHA512_DIGEST_LENGTH];
    SHA512(full_pwd, strlen(full_pwd), hash);
    printf("%d", cc->length);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        hash[i] = cc->characters[abs(hash[i]) % cc->length];
    }
    char *pwd = (char*)malloc(sizeof(char)*pwd_length + 1);
    strncpy(pwd, hash, pwd_length);
    pwd[pwd_length] = '\0';
    return pwd;
}

void setup_custom_characters(CustomCharacters *cc) {
    cc->characters = malloc(300 * sizeof(char));
    cc->length = 0;
}

void destroy_custom_characters(CustomCharacters *cc) {
    free(cc->characters);
}

void set_custom_characters(CustomCharacters *cc, char* characters) {
    strncpy(cc->characters, characters, strlen(characters));
    cc->characters[strlen(characters)] = '\0';
    cc->length = strlen(characters);
}