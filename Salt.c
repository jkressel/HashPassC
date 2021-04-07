#include "Salt.h"
#include <openssl/rand.h>
#include <string.h> 
#include <stdlib.h>

/**
 * generate acceptable salt
 * @param salt
 */
void generate_salt(Salt *salt) {
    RAND_bytes(salt->salt, 64);
    for (int i = 0; i < 64; i++){
        salt->salt[i] = (abs(salt->salt[i]) % 93) + 33; 
    }
    salt->salt[64] = '\0';
}

/**
 * get salt as a string
 * @param salt
 * @return salt
 */
char* get_salt_str(Salt *salt){
    return salt->salt;
}

/**
 * set a string as salt
 * @param salt
 * @param salt_str
 */
void set_salt(Salt *salt, unsigned char *salt_str) {
    strncpy(salt->salt, salt_str, 64);
    salt->salt[64] = '\0';
}

/**
 * allocate memory for salt
 * @param salt
 */
void setup_salt(Salt *salt) {
    salt->salt = malloc(65 * sizeof(char));
}

/**
 * free salt
 * @param salt
 */
void destroy_salt(Salt *salt) {
    free(salt->salt);
}