#include "Phrase.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

    Phrase phrase;
    phrase.word1 = (char*)malloc(5 * sizeof(char));
    phrase.word2 = (char*)malloc(5 * sizeof(char));
    phrase.word3 = (char*)malloc(5 * sizeof(char));
    phrase.word4 = (char*)malloc(5 * sizeof(char));
    phrase.word5 = (char*)malloc(5 * sizeof(char));
    phrase.word6 = (char*)malloc(5 * sizeof(char));

    printf("%d", isValidPhrase(&phrase));

    phrase.word1 = "Hi";
    phrase.word2 = "Hi";
    phrase.word3 = "Hi";
    phrase.word4 = "Hi";
    phrase.word5 = "Hi";
    phrase.word6 = "Hi";

    printf("%d", isValidPhrase(&phrase));

    printf("%s", getPhrase(&phrase));

    return 0;
}