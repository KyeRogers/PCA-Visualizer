#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>

class Matrix {
  public:
    Matrix(const size_t rows, const size_t cols);

    size_t Rows() const;
    size_t Cols() const;

    double& operator()(size_t row, size_t col);
    const double& operator()(size_t row, size_t col) const;


  private:
    size_t rows_;
    size_t cols_;
    std::vector<double> data_; 
};

#endif