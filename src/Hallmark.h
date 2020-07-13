#pragma once

#include "Constants.h"
#include <string>
#include <vector>
#include <set>

using namespace std;

class Hallmark {
private:
    string title;
    string description;
    const unsigned int index;
    WordSet* global_words;

public:
    WordAndIndexVector words;
    Hallmark(const string title, const string description, int index, set<string> * in_words);
    void setWords(set<string>);
    static void generate_owned_words(std::vector<Hallmark> *);
    WordSet generate_words();
    void restrict_owned_words(WordAndCountVector *, const unsigned int);

};
