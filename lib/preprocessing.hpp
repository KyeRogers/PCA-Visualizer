#ifndef PREPROCESSING_HPP
#define PREPROCESSING_HPP

#include <vector>

#include "matrix.hpp"

struct CenteringResult {
    Matrix centered;
    std::vector<double> means;
};

CenteringResult CenterData(const Matrix& data);

#endif