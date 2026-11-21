#include "matrix.hpp"

#include <stdexcept>

Matrix::Matrix(int rows, int cols) {
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Matrix dimensions must be positive");
  }

  rows_ = static_cast<std::size_t>(rows);
  cols_ = static_cast<std::size_t>(cols);

  data_.resize(rows_ * cols_, 0.0);
}

size_t Matrix::Rows() const { return rows_; }

size_t Matrix::Cols() const { return cols_; }

Matrix Matrix::Transpose() const {
  Matrix transposed{static_cast<int>(Cols()), static_cast<int>(Rows())};
  for (int row{0}; row < Rows(); row++) {
    for (int col{0}; col < Cols(); col++) {
      transposed(col, row) = (*this)(row, col);
    }
  }
  return transposed;
}

double& Matrix::operator()(size_t row, size_t col) {
  if (row >= rows_ || col >= cols_ || row < 0 || col < 0) {
    throw std::out_of_range("Invalid column or row");
  }
  return data_[row * cols_ + col];
}
const double& Matrix::operator()(size_t row, size_t col) const {
  if (row >= rows_ || col >= cols_ || row < 0 || col < 0) {
    throw std::out_of_range("Invalid column or row");
  }
  return data_[row * cols_ + col];
}

bool Matrix::operator==(const Matrix& other) const {
  constexpr double epsilon = 1e-10;
  if (Rows() == other.Rows() && Cols() == other.Cols()) {
    // check elements
    for (int row{0}; row < Rows(); row++) {
      for (int column{0}; column < Cols(); column++) {
        if (std::abs((*this)(row, column) - other(row, column)) > epsilon)
          return false;
      }
    }
    return true;
  }
  return false;
}


