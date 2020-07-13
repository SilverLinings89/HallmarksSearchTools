#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by kraft on 05.08.19.
//

#include <iostream>
#include <cstring>
#include <algorithm>
#include <bits/stdc++.h>
#include "Hallmark.h"

bool pair_a_lt_pair_b(WordAndCount a, WordAndCount b) {
    return a.count < b.count;
}

Hallmark::Hallmark(const string in_title, const string in_description, int in_index, WordSet * in_global_words):index(in_index) {
    title = in_title;
    description = in_description;
    global_words = in_global_words;
}

void Hallmark::generate_owned_words(std::vector<Hallmark> * in_hallmarks) {
    vector<WordSet> local_words_before, local_words_after;
    const unsigned int  n_hallmarks = in_hallmarks->size();
    local_words_before.resize(n_hallmarks);
    local_words_after.resize(n_hallmarks);
    for(unsigned int i = 0; i < n_hallmarks; i++ ) {
        local_words_before[i] = in_hallmarks->operator[](i).generate_words();
        local_words_after[i] = local_words_before[i];
        cout << "Hallmark " << i << " has " << local_words_before[i].size() << " words before removing doubles.";
    }

    cout << "Word sets generated." << endl;

    for(unsigned int i = 0; i < n_hallmarks; i++) {
        for(unsigned int j = 0; j < n_hallmarks; j++) {
            if(i != j) {
                WordSet to_erase;
                for(auto it = local_words_after[i].begin(); it != local_words_after[i].end(); it++) {
                    if(local_words_before[j].find(*it) != local_words_before[j].end()) {
                        to_erase.insert(*it);
                    }
                }
                for(auto it = to_erase.begin(); it != to_erase.end(); it++) {
                    local_words_after[i].erase(*it);
                }
            }
        }
    }

    cout << "Word sets cleaned." << endl;

    for(unsigned int i=0; i < in_hallmarks->size(); i++) {
        in_hallmarks->operator[](i).setWords(local_words_after[i]);
        cout << "Hallmark " << i << " has the following words: " <<endl;
        for(auto it = local_words_after[i].begin(); it != local_words_after[i].end(); it++) {
            cout << *it << endl;
        }
    }

    cout << "All Hallmarks have been prepared." << endl;
}

void Hallmark::setWords(WordSet in_set) {
    words.clear();
    unsigned int index = 0;
    for(auto it = this->global_words->begin(); it != this->global_words->end(); it++) {
        if(in_set.find(*it) != in_set.end()) {
            WordAndIndex wac(*it, index);
            this->words.push_back(wac);
        }
        index ++;
    }
}

WordSet Hallmark::generate_words() {
    WordSet ret;
    char chars[] = "()-_!.,;:+#?";
    for(unsigned int i = 0; i < strlen(chars); i++) {
        this->description.erase(std::remove(this->description.begin(), this->description.end(), chars[i]), this->description.end());
    }
    string delimiter = " ";
    size_t pos = this->description.find(delimiter);
    std::string token;
    unsigned int count = 0;
    while (pos != std::string::npos) {
        token = this->description.substr(0, pos);
        if((ret.find(token) == ret.end()) && (this->global_words->find(token) != this->global_words->end())) {
            ret.insert(token);
        }
        this->description.erase(0, pos + delimiter.length());
        pos = this->description.find(delimiter);
        count++;
    }
    cout << "A total of " << count << " Blocks were considered for Hallmark " << this->index+1 << endl;

    return ret;
}

void Hallmark::restrict_owned_words(WordAndCountVector * in_Counts, const unsigned int max_words) {
    WordSet words_after;
    WordAndCountVector local_counts;
    unsigned int i = 0;
    for(auto it = this->words.begin(); it != this->words.end(); it++) {
        for(; i < in_Counts->size(); i++){
            if((*it).word == in_Counts->operator[](i).word) {
                WordAndCount wac(it->word);
                wac.count = in_Counts->operator[](i).count;
                local_counts.push_back(wac);
            }
        }
    }

    sort(local_counts.begin(), local_counts.end(), pair_a_lt_pair_b);

    const unsigned final_count = std::min<unsigned int>(max_words, local_counts.size());

    for(unsigned int i = 0; i < final_count; i++) {
        words_after.insert(local_counts[i].word);
    }

    this->setWords(words_after);
}

#pragma clang diagnostic pop