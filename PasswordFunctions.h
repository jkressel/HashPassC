#ifndef PWD_H   /* Include guard */
#define PWD_H

typedef struct CustomCharacters
{ 
    int length;
    char* characters;
    
}CustomCharacters;

char* getPassword(CustomCharacters *cc, int pwdLength);

#endif // PWD_H_