//
// Created by alistair on 09/04/2021.
//
#include <assert.h>
#include "../Phrase.h"
#include <stdio.h>
#include <string.h>

void setup(Phrase *phrase) {
    setup_phrase(phrase);
}

void test_valid_phrase_is_valid(Phrase *phrase) {
    strncpy(phrase->word1, "word1", 10);
    strncpy(phrase->word2, "word2", 10);
    strncpy(phrase->word3, "word3", 10);
    strncpy(phrase->word4, "word4", 10);
    strncpy(phrase->word5, "word5", 10);
    strncpy(phrase->word6, "word6", 10);

    assert(!is_valid_phrase(phrase));
    printf("%s\n", "Valid phrase test produces valid result");
}

void test_invalid_phrase_is_invalid(Phrase *phrase) {
    strncpy(phrase->word1, "word1", 10);
    strncpy(phrase->word1, "\0", 10);
    strncpy(phrase->word3, "word3", 10);
    strncpy(phrase->word4, "word4", 10);
    strncpy(phrase->word5, "word5", 10);
    strncpy(phrase->word6, "word6", 10);

    assert(is_valid_phrase(phrase));
    printf("%s\n", "Invalid phrase test produces valid result");
}

void test_phrases_match_recognises_two_identical_phrases(Phrase *phrase1, Phrase *phrase2) {
    strncpy(phrase1->word1, "word1", 10);
    strncpy(phrase1->word2, "word2", 10);
    strncpy(phrase1->word3, "word3", 10);
    strncpy(phrase1->word4, "word4", 10);
    strncpy(phrase1->word5, "word5", 10);
    strncpy(phrase1->word6, "word6", 10);

    strncpy(phrase2->word1, "word1", 10);
    strncpy(phrase2->word2, "word2", 10);
    strncpy(phrase2->word3, "word3", 10);
    strncpy(phrase2->word4, "word4", 10);
    strncpy(phrase2->word5, "word5", 10);
    strncpy(phrase2->word6, "word6", 10);

    assert(!phrases_match(phrase1, phrase2));
    printf("%s\n", "Two identical phrases are recognised as being identical");
}

int main () {
    Phrase phrase;
    Phrase phrase1;
    setup(&phrase);
    test_valid_phrase_is_valid(&phrase);
    destroy_phrase(&phrase);
    setup(&phrase);
    test_invalid_phrase_is_invalid(&phrase);
    destroy_phrase(&phrase);
    setup(&phrase);
    setup(&phrase1);
    test_phrases_match_recognises_two_identical_phrases(&phrase, &phrase1);
    destroy_phrase(&phrase);
    destroy_phrase(&phrase1);
    printf("\n%s\n", "All tests successful");
    return 0;
}

