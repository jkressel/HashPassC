#include "Phrase.h"
#include <string.h>
#include <stdlib.h>

const int MAX_WORD_LENGTH = 100;
const int NUMBER_OF_WORDS = 6;


/**
 * check the phrase is valid
 * @param phrase
 * @return 0 if valid, 1 if not
 */
int is_valid_phrase(Phrase *phrase) {
    if ((strlen(phrase->word1) > 0) && (strlen(phrase->word2) > 0) && (strlen(phrase->word3) > 0) && (strlen(phrase->word4) > 0) &&
            (strlen(phrase->word5) > 0) && (strlen(phrase->word6) > 0))
            return 0;
    return 1;
}

/**
 * get phrase as string
 * @param phrase
 * @return phrase
 */
char* get_phrase(Phrase *phrase) {
    char* whole_phrase = (char*)malloc(strlen(phrase->word1) + strlen(phrase->word2) + strlen(phrase->word3) + strlen(phrase->word4) +
             strlen(phrase->word5) + strlen(phrase->word6) + 1);

    strcpy(whole_phrase, phrase->word1);
    strcat(whole_phrase, phrase->word2);
    strcat(whole_phrase, phrase->word3);
    strcat(whole_phrase, phrase->word4);
    strcat(whole_phrase, phrase->word5);
    strcat(whole_phrase, phrase->word6);

    return whole_phrase;
}

/**
 * allocate memory for phrase
 * @param phrase
 */
void setup_phrase(Phrase* phrase) {
    phrase->word1 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
    phrase->word2 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
    phrase->word3 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
    phrase->word4 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
    phrase->word5 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
    phrase->word6 = malloc(100 * sizeof(char) * MAX_WORD_LENGTH);
}

/**
 * free phrase
 * @param phrase
 */
void destroy_phrase(Phrase *phrase) {
    free(phrase->word1);
    free(phrase->word2);
    free(phrase->word3);
    free(phrase->word4);
    free(phrase->word5);
    free(phrase->word6);
}

/**
 * check that the phrases match
 * @param phrase1
 * @param phrase2
 * @return 0 for matching, 1 for no match
 */
int phrases_match(Phrase *phrase1, Phrase *phrase2) {
    if (!strncmp(phrase1->word1, phrase2->word1, strlen(phrase1->word1)) && !strncmp(phrase1->word2, phrase2->word2, strlen(phrase1->word2)) &&
            !strncmp(phrase1->word3, phrase2->word3, strlen(phrase1->word3)) && !strncmp(phrase1->word4, phrase2->word4, strlen(phrase1->word4)) &&
            !strncmp(phrase1->word5, phrase2->word5, strlen(phrase1->word5)) && !strncmp(phrase1->word6, phrase2->word6, strlen(phrase1->word6)))
        return 0;
    else
        return 1;
}