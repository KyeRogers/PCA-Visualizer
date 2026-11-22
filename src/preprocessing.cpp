#include "preprocessing.hpp"

CenteringResult CenterData(const Matrix& data) {

  CenteringResult result{Matrix{static_cast<int>(data.Rows()), static_cast<int>(data.Cols())}, std::vector<double>(data.Cols(), 0.0)};
  for (size_t col{0}; col < data.Cols(); col++) {
    // calculate means
    for (size_t row{0}; row < data.Rows(); row++) {
      result.means[col] += data(row, col);
    }
    result.means[col] /= data.Rows();

    // calculate matrix
    for (size_t row{0}; row < data.Rows(); row++) {
      result.centered(row, col) = data(row, col) - result.means[col]; 
    }
  }
  return result;
}
