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

const int MAX_WORD_LENGTH;
const int NUMBER_OF_WORDS;

int is_valid_phrase(Phrase *phrase);

char* get_phrase(Phrase *phrase);

void setup_phrase(Phrase* phrase);

void destroy_phrase(Phrase *phrase);

int phrases_match(Phrase *phrase1, Phrase *phrase2);



#endif