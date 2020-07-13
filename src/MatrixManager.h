#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include "Hallmark.h"
#include "Structs.h"
#include "Constants.h"
#include <string>

using namespace std;

class MatrixManager {
public:
    boost::numeric::ublas::matrix<float> cooccurences;
    boost::numeric::ublas::matrix<float> rating, old_rating;
    const unsigned int n_words;
    MatrixManager(unsigned int in_n_words);
    void LocalToGlobal(boost::numeric::ublas::matrix<float> local, std::vector<unsigned int> global_indices);
    void AddContributions(std::vector<IndexData> data);
    static void set_rating_components_for_hallmarks(boost::numeric::ublas::matrix<float> *, std::vector<Hallmark> * hallmarks);
    bool build_word_rating(std::vector<Hallmark> * hallmarks);
    static void normalize_rating_by_row(boost::numeric::ublas::matrix<float> *);
    float compare_old_to_new_rating();
    void copy_new_to_old_rating();
    void write_rating_to_file (string);
    void convergence_achieved();

    void store_matrix(string in_filename);

    bool load_matrix(string in_filename);
    void analyze_rating(boost::numeric::ublas::matrix<float> * in_rating);
};
