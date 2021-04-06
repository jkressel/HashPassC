//
// Created by alistair on 21/02/2021.
//

#ifndef HASHPASSC_CRYPTOFUNCTIONS_H
#define HASHPASSC_CRYPTOFUNCTIONS_H

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <math.h>

int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);

unsigned char *encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);

unsigned char *decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);

unsigned char *generate_crypto_salt();

#endif //HASHPASSC_CRYPTOFUNCTIONS_H
