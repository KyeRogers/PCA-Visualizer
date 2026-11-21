#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>

class Matrix {
  public:
    Matrix(int rows, int cols);

    size_t Rows() const;
    size_t Cols() const;

    double& operator()(size_t row, size_t col);
    const double& operator()(size_t row, size_t col) const;

    bool operator==(const Matrix& other) const;


  private:
    size_t rows_;
    size_t cols_;
    std::vector<double> data_; 
};

#endif