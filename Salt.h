#ifndef SALT_H   /* Include guard */
#define SALT_H

typedef struct Salt 
{ 
    unsigned char *salt;
    
}Salt;

char* get_salt_str(Salt *salt);

void generate_salt(Salt *salt);

void set_salt(Salt *salt, unsigned char* salt_str);

void setup_salt(Salt *salt);

void destroy_salt(Salt *salt);


#endif