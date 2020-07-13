#include "WordHistogram.h"
#include "Constants.h"
#include <algorithm>

WordHistogram::WordHistogram() {
    data_ = {};
    is_sorted = true;
}

auto WordHistogram::sort() -> void {
    std::sort(data_.begin(), data_.end());
    is_sorted = true;
}

auto WordHistogram::contains_word(const string word) -> bool {
    if( indexOfWordInData(word) == -1) {
        return false;
    } else {
        return true;
    }
}

auto WordHistogram::indexOfWordInData(string word) -> int {
    int return_value = -1;
    if(!is_sorted) {
        sort();
    }
    unsigned int pos = 0;
    while (pos < data_.size()) {
        int compare = data_[pos].word.compare(word);
        if( compare < 0) {
            pos++;
        } else {
            if( compare == 0 ) {
                return pos;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

auto WordHistogram::addWordWithCount(string word, unsigned int count) -> void {
    WordAndCount wac = {word};
    wac.count = count;
    data_.push_back(wac);
    is_sorted = false;
}

auto WordHistogram::updateWithWord(string word) -> void {
    int current_index = indexOfWordInData(word);
    if(current_index < 0) {
        addWordWithCount(word, 1);
    } else {
        data_[current_index].increment();
    }
}

auto WordHistogram::clear() -> void {
    data_.clear();
}

auto WordHistogram::getCountForWord(string word) -> unsigned int {
    int current_index = indexOfWordInData(word);
    if(current_index < 0) {
        return 0;
    } else {
        return data_[current_index].count;
    }
}

auto WordHistogram::getWordSet() -> WordSet {
    WordSet ret;
    for(auto wac : data_) {
        ret.insert(wac.word);
    }
    return ret;
}