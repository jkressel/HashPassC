//
// Created by alistair on 09/04/2021.
//
#include <assert.h>
#include "../Salt.h"
#include <stdio.h>
#include <string.h>

void setup(Salt *salt) {
    setup_salt(salt);
}

void test_salt_generates_64_bit_salt(Salt *salt) {
    generate_salt(salt);
    assert(strlen(get_salt_str(salt)) == 64);
    printf("%s\n", "Salt generated with length 64");
    assert(get_salt_str(salt)[64] == '\0');
    printf("%s\n", "Salt is null terminated");
}

int main () {
    Salt salt;
    setup(&salt);
    test_salt_generates_64_bit_salt(&salt);
    printf("\n%s\n", "All tests successful");
    destroy_salt(&salt);
    return 0;
}
