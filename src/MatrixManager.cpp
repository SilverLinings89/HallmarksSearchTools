#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by pascal on 04.08.19.
//

#include "MatrixManager.h"
#include <boost/numeric/ublas/operation.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/karma.hpp>
#include "Constants.h"
#include <fstream>
#include <cmath>

void
MatrixManager::LocalToGlobal(boost::numeric::ublas::matrix<float> local, std::vector<unsigned int> global_indices) {
    for(unsigned int i= 0; i < global_indices.size(); i++) {
        for(unsigned int j = 0; j < global_indices.size(); j++) {
            this->cooccurences(global_indices[i], global_indices[j]) += local(i,j);
        }
    }
}

MatrixManager::MatrixManager(unsigned int in_n_words):n_words(in_n_words) {
    this->cooccurences.resize(n_words, n_words);
}

void MatrixManager::AddContributions(std::vector<IndexData> data) {
    for(unsigned int i= 0; i < data.size(); i++) {
        for(unsigned int j = 0; j < data.size(); j++) {
            this->cooccurences(data[i].globalIndex, data[j].globalIndex) += log((data[i].occurences * data[j].occurences)*e);
        }
    }
}

bool MatrixManager::build_word_rating(std::vector<Hallmark> *hallmarks) {
    this->rating.resize(n_words, hallmarks->size());
    this->old_rating.resize(n_words, hallmarks->size());
    this->old_rating.clear();
    set_rating_components_for_hallmarks(&rating, hallmarks);
    set_rating_components_for_hallmarks(&old_rating, hallmarks);
    boost::numeric::ublas::vector<float> input(n_words);
    boost::numeric::ublas::vector<float> output(n_words);
    unsigned int step = 0;
    bool is_converged = false;
    while(step < MATRIX_ITERATION_STEPS && !is_converged) {
        for(unsigned int i = 0; i < hallmarks->size(); i++) {

#pragma omp parallel for
            for(unsigned int j = 0; j < n_words; j++) {
                input(j) = old_rating(j,i);
            }

            axpy_prod(cooccurences, input, output);

#pragma omp parallel for
            for(unsigned int j = 0; j < n_words; j++) {
                rating(j,i) = output(j);
                output(j) = 0;
            }
        }

        set_rating_components_for_hallmarks(&rating, hallmarks);
        normalize_rating_by_row(&rating);
        float residual  = compare_old_to_new_rating();
        analyze_rating(&rating);
        if ( residual < RESIDUAL_LOWER_BOUND) {
            cout << "Convergence has been reached." << endl;
            convergence_achieved();
            is_converged = true;
        } else {
            copy_new_to_old_rating();
            step++;
        }
    }
    return is_converged;
}



void MatrixManager::set_rating_components_for_hallmarks(boost::numeric::ublas::matrix<float> * in_rating,
                                                        std::vector<Hallmark> *hallmarks) {
    for(unsigned int h = 0; h < hallmarks->size(); h++) {
        for(unsigned int word_index = 0; word_index < hallmarks->operator[](h).words.size(); word_index++ ){
            for(unsigned int i = 0; i < hallmarks->size(); i++){
                if(i == h) {
                    in_rating->operator()(hallmarks->operator[](h).words[word_index].index, i) = 1.0;
                } else {
                    in_rating->operator()(hallmarks->operator[](h).words[word_index].index, i) = 0.0;
                }
            }
        }
    }
}

void MatrixManager::normalize_rating_by_row(boost::numeric::ublas::matrix<float> * in_rating) {

#pragma omp parallel for
    for(unsigned int i = 0; i < in_rating->size1(); i++) {
        float norm = 0.0;
        for(unsigned int j = 0; j < in_rating->size2(); j++) {
            norm += in_rating->operator()(i,j)*in_rating->operator()(i,j);
        }
        norm = sqrt(norm);
        if(norm != 0.0) {
            for (unsigned int j = 0; j < in_rating->size2(); j++) {
                in_rating->operator()(i, j) = in_rating->operator()(i, j) / norm;
            }
        }
    }
}

void MatrixManager::copy_new_to_old_rating() {
    old_rating = rating;
}

float MatrixManager::compare_old_to_new_rating() {
    double delta = 0.0;
    double l_infty_dif = 0.0;

#pragma omp parallel for
    for(unsigned int i = 0; i < rating.size1(); i++) {
        for(unsigned int j = 0; j < rating.size2(); j++) {
            double loc_delta = rating(i,j) - old_rating(i,j);
            if (l_infty_dif < std::abs(loc_delta)) l_infty_dif = std::abs(loc_delta);
            delta += loc_delta*loc_delta;
        }
    }

    delta = std::sqrt(delta);
    std::cout << "L_infinity norm of difference: " << l_infty_dif << std::endl;
    std::cout << "L_2 norm of difference: " << delta << std::endl;
    return delta;
}

void MatrixManager::write_rating_to_file(std::string filename) {

        ofstream out_stream;
        out_stream.open(filename, std::ofstream::out | std::ofstream::trunc);

        for (unsigned int i = 0; i < rating.size1(); i++) {
            for (unsigned int j = 0; j < rating.size2() - 1; j++) {
                out_stream << rating(i, j) << " ";
            }
            out_stream << rating(i, rating.size2() - 1) << endl;
        }
        out_stream.flush();
        out_stream.close();
}

void MatrixManager::convergence_achieved() {
    write_rating_to_file(RATING_OUTPUT_FILENAME);
}

void MatrixManager::store_matrix(string in_filename) {
    if(( access( in_filename.c_str(), F_OK ) != -1 )) {
        cout << "File already exists. Not storing again." << endl;
    } else {
        ofstream out_stream;
        out_stream.open(in_filename, std::ofstream::out | std::ofstream::trunc);

        for (unsigned int i = 0; i < cooccurences.size1(); i++) {
            for (unsigned int j = 0; j < cooccurences.size2() - 1; j++) {
                out_stream << cooccurences(i, j) << " ";
            }
            out_stream << cooccurences(i, cooccurences.size2() - 1) << endl;
        }
        out_stream.flush();
        out_stream.close();
    }
}

bool MatrixManager::load_matrix(string in_filename) {
    ifstream in_stream(in_filename);
    if(in_stream.good()) {
        string line;
        int line_index = 0;
        while (std::getline(in_stream, line)) {
            stringstream linestream(line);
            string part;
            int row_index = 0;
            while (std::getline(linestream, part, ' ')) {
                cooccurences(line_index, row_index) = stof(part);
                row_index++;
            }
            line_index++;
        }
        return true;
    } else {
        return false;
    }
}

void MatrixManager::analyze_rating(boost::numeric::ublas::matrix<float> * in_rating) {
    const float stepwidth = 1.0 / (float)N_PARTS_HISTOGRAM;
    ofstream out_stream;
    out_stream.open(HISTOGRAM_FILE_NAME, std::ofstream::out);
    int ** complete_histogram = new int * [in_rating->size2()];
    for(unsigned int i = 0; i < in_rating->size2(); i++) {
        complete_histogram[i] = new int[N_PARTS_HISTOGRAM+1];
    }
    for(unsigned int hallmark = 0; hallmark < in_rating->size2(); hallmark++) {
        for(unsigned int i = 0; i < N_PARTS_HISTOGRAM+1; i++) complete_histogram[hallmark][i] = 0;
        for(unsigned int row = 0; row < in_rating->size1(); row++) {
            complete_histogram[hallmark][(int)((*in_rating).operator()(row, hallmark) / stepwidth)]++;
        }
    }
    out_stream << std::endl;
    for(unsigned int hallmark = 0; hallmark < in_rating->size2(); hallmark++) {
        out_stream << "|";
        for(unsigned int i = 0; i < N_PARTS_HISTOGRAM+1; i++) {
            out_stream << complete_histogram[hallmark][i] << "\t";
        }
    }
    return;
}

#pragma clang diagnostic pop