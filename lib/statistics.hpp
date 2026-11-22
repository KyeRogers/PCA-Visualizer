#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include "matrix.hpp"

// store the means and the new matrix
struct CenteringResult {
    Matrix centered;
    std::vector<double> means;
};

// center the data by subtracting each value by that columns mean
CenteringResult CenterData(const Matrix& data);
// calculate the covariance matrix
Matrix ComputeCovariance(const Matrix& centeredData);

struct EigenResult {
    std::vector<double> eigenvalues;
    Matrix eigenvectors;
};

EigenResult EigenDecomposeSymmetric2x2(const Matrix& matrix);
#endif