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
Matrix ComputeCovariance(const Matrix& centeredData);

#endif