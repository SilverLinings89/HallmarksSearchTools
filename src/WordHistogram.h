#pragma once

#include <string>
#include "Constants.h"
#include "Structs.h"

using namespace std;
class WordHistogram {
    private:
        WordAndCountVector data_;
        bool is_sorted;

    public:
    
        WordHistogram();
    
        auto addWordWithCount(string word, unsigned int count) -> void;
        auto updateWithWord(string word) -> void;
        auto clear() -> void;
        auto getCountForWord(string word) -> unsigned int;
        auto sort() -> void;
        auto print_short() -> string;
        auto print_long() -> long;
        auto contains_word(const string word) -> bool;
        auto indexOfWordInData(const string word) -> int;
        auto getWordSet() -> WordSet;
};
