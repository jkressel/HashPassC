#ifndef PWD_H   /* Include guard */
#define PWD_H

typedef struct CustomCharacters
{ 
    int length;
    char* characters;
    
}CustomCharacters;

char DEFAULT_CHARS[100];

char* get_password(CustomCharacters *cc, int pwd_length, char* full_pwd);

void setup_custom_characters(CustomCharacters *cc);

void destroy_custom_characters(CustomCharacters *cc);

void set_custom_characters(CustomCharacters *cc, char* characters);

#endif // PWD_H_