#pragma once

#include <string>

using namespace std;

struct IndexData {
    unsigned int globalIndex;
    unsigned int occurences;
};

struct WordAndIndex {
    string word;
    unsigned int index;
    WordAndIndex(const string in_word, const unsigned int in_index):word(in_word), index(in_index) { }
};

struct WordAndCount {
    string word;
    unsigned int count;
    
    WordAndCount():word("") {
        count = 0;
    }

    WordAndCount(string in_word): word(in_word) {
        count = 0;
    }
    
    auto increment() -> void {
        count++;
    }
    
    bool operator < (const WordAndCount& str) const
    {
        return word.compare(str.word) < 0;
    }
};
