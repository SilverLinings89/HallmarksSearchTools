#include <iostream>
#include "Paper.h"
#include "Constants.h"
#include "WordHistogram.h"

using namespace std;

Paper::Paper(string in_uid, string in_title, string in_abstract , unsigned int in_index):
title(in_title), abstract(in_abstract), uid(in_uid), index(in_index) {
    PrepareAbstract();
}



auto Paper::OccuringWordsFromWordSet(WordSet * in_Words) -> WordSet {
    WordSet ret;
    string temp_abstract = abstract;
    const string delimiter = " ";
    size_t pos = temp_abstract.find(delimiter);
    string token;
    while ( pos != string::npos) {
        token = temp_abstract.substr(0, pos);
        if(in_Words->contains(token)) {
            ret.insert(token);
        }
        temp_abstract.erase(0, pos + delimiter.length());
        pos = temp_abstract.find(delimiter);
    }
    return ret;
}

auto Paper::GetWordHistogram(WordSet * in_Words) -> WordHistogram {
    WordHistogram histogram;
    WordSet found_words;
    string temp_abstract = abstract;
    const string delimiter = " ";
    string word;
    for ( size_t pos = temp_abstract.find(delimiter); pos != string::npos; pos = temp_abstract.find(delimiter)) {
      word = temp_abstract.substr(0, pos);
      if (in_Words->contains(word)) {
        histogram.updateWithWord(word);
      }
      temp_abstract.erase(0, pos + delimiter.length());
    }
    histogram.sort();
    return histogram;
}

auto Paper::RemoveIllegalCharactersFromAbstract() -> void {
    abstract.erase(
        std::remove_if(
            abstract.begin(), 
            abstract.end(),
            [](char c) { return isIllegalChar(c); } 
        ),
        abstract.end()
    );
    return;
}

auto Paper::MakeAbstractWordsLowerCase() -> void {
    if (abstract.size() != 0) {
        if(isupper(abstract[0])) {
            abstract[0] = tolower(abstract[0]);
        }
    }
    for(unsigned int i = 2; i < abstract.size(); i++) {
        if(abstract[i - 2] == ' ' && isupper(abstract[i-1] && islower(abstract[i]))) {
            abstract[i - 1] = tolower(abstract[i-1]);
        }
    }
    return;
}

auto Paper::PrepareAbstract() -> void {
    
    RemoveIllegalCharactersFromAbstract();
    MakeAbstractWordsLowerCase();
    abstract = AppendSpaceIfMissing(abstract);
}

auto Paper::AppendSpaceIfMissing(const string in_text) -> string {
    string return_value = in_text;
    if(return_value.size() > 0) {
        if(return_value[return_value.size()-1] != ' ') {
            return_value.append(" ");
        }
    }
    return return_value;
}
