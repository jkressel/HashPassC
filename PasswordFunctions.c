#include "PasswordFunctions.h"
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>


char DEFAULT_CHARS[] = "!\"#$%&'()*+,-./0123456789:;<>=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\^_`abcdefghijklmnopqrstuvwxyz{}|~";

/**
 * generate that password!
 * @param cc
 * @param pwd_length
 * @param full_pwd
 * @return password at required length using specified characters
 */
char* get_password(CustomCharacters* cc, int pwd_length, char* full_pwd) {
    char hash[SHA512_DIGEST_LENGTH];
    SHA512(full_pwd, strlen(full_pwd), hash);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        hash[i] = cc->characters[abs(hash[i]) % cc->length];
    }
    char *pwd = (char*)malloc(sizeof(char)*pwd_length + 1);
    strncpy(pwd, hash, pwd_length);
    pwd[pwd_length] = '\0';
    return pwd;
}

/**
 * allocate memory for the characters to use
 * @param cc
 */
void setup_custom_characters(CustomCharacters *cc) {
    cc->characters = malloc(300 * sizeof(char));
    cc->length = 0;
}

/**
 * free characters
 * @param cc
 */
void destroy_custom_characters(CustomCharacters *cc) {
    free(cc->characters);
}

/**
 * initialise the character set to use
 * @param cc
 * @param characters
 */
void set_custom_characters(CustomCharacters *cc, char* characters) {
    strncpy(cc->characters, characters, strlen(characters));
    cc->characters[strlen(characters)] = '\0';
    cc->length = strlen(characters);
}