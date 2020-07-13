#pragma once
#include "Constants.h"
#include "WordHistogram.h"

using namespace std;

class Paper {
    const string title;
    string abstract;
    const string uid;
    unsigned int index;

public:
    Paper(string, string, string, unsigned int);
    auto OccuringWordsFromWordSet(WordSet *) -> WordSet;
    auto GetWordHistogram(WordSet *) -> WordHistogram;
    auto PrepareAbstract() -> void;
    auto MakeAbstractWordsLowerCase() -> void;
    auto RemoveIllegalCharactersFromAbstract() -> void;
    auto AppendSpaceIfMissing(const string) -> string;
};
