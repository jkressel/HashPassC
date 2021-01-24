#ifndef PHRASE_H   /* Include guard */
#define PHRASE_H

typedef struct Phrase 
{ 
    char* word1;
    char* word2;
    char* word3;
    char* word4;
    char* word5;
    char* word6;
    
}Phrase;

int isValidPhrase(Phrase *phrase);

char* getPhrase(Phrase *phrase);

#endif