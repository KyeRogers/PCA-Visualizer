#include "matrix.hpp"

Matrix::Matrix(const size_t rows, const size_t cols)
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {}

size_t Matrix::Rows() const {
    return 0;
}

size_t Matrix::Cols() const {
    return 0;
}

double& Matrix::operator()(size_t row, size_t col) {
    return data_[0];
}

const double& Matrix::operator()(size_t row, size_t col) const {
    return data_[0];
}