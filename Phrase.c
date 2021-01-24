#include "Phrase.h"
#include <string.h>
#include <stdlib.h>

int isValidPhrase(Phrase *phrase) {
    if ((strlen(phrase->word1) > 0) && (strlen(phrase->word2) > 0) && (strlen(phrase->word3) > 0) && (strlen(phrase->word4) > 0) &&
            (strlen(phrase->word5) > 0) && (strlen(phrase->word6) > 0))
            return 1;
    return 0;
}

char* getPhrase(Phrase *phrase) {
    char* wholePhrase = (char*)malloc(strlen(phrase->word1) + strlen(phrase->word2) + strlen(phrase->word3) + strlen(phrase->word4) +
             strlen(phrase->word5) + strlen(phrase->word6) + 1);

    strcpy(wholePhrase, phrase->word1);
    strcat(wholePhrase, phrase->word2);
    strcat(wholePhrase, phrase->word3);
    strcat(wholePhrase, phrase->word4);
    strcat(wholePhrase, phrase->word5);
    strcat(wholePhrase, phrase->word6);

    return wholePhrase;
}