//
// Created by alistair on 21/02/2021.
//

#ifndef HASHPASSC_CRYPTOFUNCTIONS_H
#define HASHPASSC_CRYPTOFUNCTIONS_H

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <math.h>
#include "Config.h"

unsigned char *generate_crypto_salt();

unsigned char *init_and_decrypt(char *ciphertext, int *len, Config *config, char *crypto_salt);
unsigned char *init_and_encrypt(char *plaintext, int *len, Config *config, char *crypto_salt);

#endif //HASHPASSC_CRYPTOFUNCTIONS_H
