#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../src/Paper.h"
#include "../src/MatrixManager.h"
#include "../src/Constants.h"
#include "../src/Hallmark.h"
#include "../src/Timer.h"
#include "../src/WordHistogram.h"

using namespace std;

static bool wordHasNoIllegalChars(string word) {
    if (word.find("_") != string::npos) {
        return false;
    }
    if (word.find("&") != string::npos) {
        return false;
    }
    if (word.find("#") != string::npos) {
        return false;
    }
    if (word.find("$") != string::npos) {
        return false;
    }
    return true;
}


int main() {
    ifstream WordFile(WORD_FILE_NAME);
    ifstream PaperFile(PAPER_FILE_NAME);
    vector<Paper *> Papers;
    int nPapers = 0;
    set<string> WordsSet;

    // Read Words File
    string line;
    while (getline(WordFile, line)) {
        if (wordHasNoIllegalChars(line)) {
            size_t p = line.find(CARR_RET);
            if(p!=string::npos) line.erase(p, CARR_RET.length());
            WordsSet.insert(line);
        }
    }
    cout << "I found " << WordsSet.size() << " Words."<< endl;

    // Put words in an alphabetical list with pair<string, int>
    WordAndCountVector WordsAndCounts;
//#pragma omp for
    for (auto it = WordsSet.begin(); it != WordsSet.end(); ++it) {
        WordAndCount temp = { *it };
        WordsAndCounts.push_back(temp);
    }

    // Count Papers in File
    while (getline(PaperFile, line)) {
        nPapers++;
    }
    PaperFile.close();
    cout << "I found " << nPapers << " Papers." << endl;
    Papers.resize(nPapers);
    PaperFile.open(PAPER_FILE_NAME);
    cout << "Resized Papers List and reopened Papers File." << endl;


    // Analyze Papers
    const string delimiter = "\t";
    float progress = 0.0;
    unsigned int done_Papers = 0;

    for(int i = 0; i < nPapers; i++) {
        string paperLine,uid,title,abstract;
        uid = "";
        title = "";
        abstract = "";
        getline(PaperFile, paperLine);

        unsigned int pos = 0;
        pos = paperLine.find(delimiter);
        if(pos != std::string::npos) {
            uid = paperLine.substr(0, pos);
            paperLine.erase(0, pos + delimiter.length());
        }

        pos = paperLine.find(delimiter);
        if(pos != std::string::npos) {
            title = paperLine.substr(0, pos);
            paperLine.erase(0, pos + delimiter.length());
        }

        pos = paperLine.find(delimiter);
        if(pos != std::string::npos) {
            abstract = paperLine.substr(0, pos);
            paperLine.erase(0, pos + delimiter.length());
        }

        if(uid != "" && title != "" && abstract !="") {
            Papers[i] = new Paper(uid, title, abstract, i);
        } else {
            cout << "A Paper line was malformated in the input file. Index: "<< i <<endl;
        }

        if(i%100 == 0) {
            progress = (float)i/(float)nPapers;
            std::cout << "[";
            int pos = BAR_WIDTH * progress;
            for (int j = 0; j < BAR_WIDTH; ++j) {
                if (j < pos) std::cout << "=";
                else if (j == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << progress * 100.0 << " % [" << i << "/" << nPapers<< "]\r";
            std::cout.flush();
        }
    }

    cout << endl << "All papers have been read to memory." << endl;
    progress = 0.0;
#pragma omp parallel for
    for (unsigned int i = 0; i < Papers.size(); i++) {
        set<string> OccuringWordsInPaper = Papers[i]->OccuringWordsFromWordSet(&WordsSet);
        for(unsigned int j = 0; j < WordsAndCounts.size(); j++) {
            if(OccuringWordsInPaper.find(WordsAndCounts[j].word) != OccuringWordsInPaper.end()) {
                WordsAndCounts[j].increment();
            }
        }
        done_Papers++;

        if(i%100 == 0) {
            progress = (float)done_Papers/(float)nPapers;
            std::cout << "[";
            int pos = BAR_WIDTH * progress;
            for (int j = 0; j < BAR_WIDTH; ++j) {
                if (j < pos) std::cout << "=";
                else if (j == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << progress * 100.0 << " % [" << done_Papers << "/" << nPapers<< "]\r";
            std::cout.flush();
        }
    }
    std::cout << std::endl;
    cout << "Done Loading all papers." << endl;

    int cnt = 0;
    for(unsigned int i = 0; i < WordsAndCounts.size(); i++) {
        if(WordsAndCounts[i].count > 10) cnt++;
    }
    cout << "There are " << cnt << " Words that occur at least 11 times." << endl;
    // WordsAndCounts are now calculated.
    const int MinOccurences = LOWER_THRESHOLD_PERCENTAGE * nPapers;
    const int MaxOccurences = UPPER_THRESHOLD_PERCENTAGE * nPapers;
    set<string> ConsideredWords;
    for(unsigned int i = 0; i < WordsAndCounts.size(); i++) {
        if(WordsAndCounts[i].count >= MinOccurences && WordsAndCounts[i].count <= MaxOccurences) {
            ConsideredWords.insert(WordsAndCounts[i].word);
        }
    }
    cout << "I will consider a total of " << ConsideredWords.size() << " words. They occure at least " << MinOccurences
    << " times and at most " << MaxOccurences << " times." << endl;
    const unsigned int nWords = ConsideredWords.size();
    vector<string> Words;
    for(auto it = ConsideredWords.begin(); it != ConsideredWords.end(); it++) {
        Words.push_back(*it);
    }
    MatrixManager mm(nWords);
    int done_papers = 0;
    if(! mm.load_matrix(MATRIX_FILE_NAME)) {
#pragma omp parallel for
        for (unsigned int i = 0; i < nPapers; i++) {
            WordHistogram histogram = Papers[i]->GetWordHistogram(&ConsideredWords);
            vector<IndexData> LocalMatrixContributionData;
            for (auto w : histogram.getWordSet()) {
                vector<string>::iterator it = find(Words.begin(), Words.end(), w);
                IndexData temp;
                temp.globalIndex = distance(Words.begin(), it);
                temp.occurences = histogram.getCountForWord(w);
                LocalMatrixContributionData.push_back(temp);
            }
            mm.AddContributions(LocalMatrixContributionData);
            done_papers++;
            if (done_papers % 100 == 0) {
                progress = (float) done_papers / (float) nPapers;
                std::cout << "[";
                int pos = BAR_WIDTH * progress;
                for (int j = 0; j < BAR_WIDTH; ++j) {
                    if (j < pos) std::cout << "=";
                    else if (j == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << progress * 100.0 << " % [" << done_papers << "/" << nPapers << "]\r";
                std::cout.flush();
            }
        }
    }
    std::cout << std::endl;
    cout << "Done Loading all papers." << endl;
    cout << "Matrix is completely assembled." << std::endl;
    mm.store_matrix(MATRIX_FILE_NAME);
    // Load the Hallmarks

    vector<Hallmark> Hallmarks;
    Hallmarks.push_back(Hallmark(hallmark1_name, hallmark1_description, 0, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark2_name, hallmark2_description, 1, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark3_name, hallmark3_description, 2, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark4_name, hallmark4_description, 3, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark5_name, hallmark5_description, 4, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark6_name, hallmark6_description, 5, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark7_name, hallmark7_description, 6, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark8_name, hallmark8_description, 7, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark9_name, hallmark9_description, 8, & ConsideredWords));
    Hallmarks.push_back(Hallmark(hallmark10_name, hallmark10_description, 9, & ConsideredWords));

    Hallmark::generate_owned_words(& Hallmarks);

    for(unsigned int i = 0; i < Hallmarks.size(); i++) {
        Hallmarks[i].restrict_owned_words(&WordsAndCounts, MAX_HALLMARK_WORD_COUNT);
    }

    cout << "Starting to build rating: " << endl;

    bool converged = mm.build_word_rating(&Hallmarks);

    cout << "Done building rating." << endl;

    cout << "Rating written to text file (" << RATING_OUTPUT_FILENAME << ")." << endl;

    ofstream outwords;
    outwords.open(WORD_OUTPUT_FILENAME, std::ofstream::out | std::ofstream::trunc);
    for(auto it = ConsideredWords.begin(); it != ConsideredWords.end(); it++) {
        outwords << *it << endl;
    }
    outwords.close();

    cout << "Words written to text file (" << WORD_OUTPUT_FILENAME << ")."<<  endl;

    return 0;
}
#pragma clang diagnostic pop
