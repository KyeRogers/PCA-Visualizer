#include "statistics.hpp"

#include <cmath>
#include <stdexcept>

CenteringResult CenterData(const Matrix& data) {
  CenteringResult result{
      Matrix{static_cast<int>(data.Rows()), static_cast<int>(data.Cols())},
      std::vector<double>(data.Cols(), 0.0)};
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

Matrix ComputeCovariance(const Matrix& centeredData) {
  if (centeredData.Rows() <= 1) {
    throw std::invalid_argument(
        "Covariance Matrix calculation requires at least 2 observations");
  }
  Matrix result = centeredData.Transpose() * centeredData *
                  (1.0 / (static_cast<double>(centeredData.Rows()) - 1.0));
  return result;
}

EigenResult EigenDecomposeSymmetric2x2(const Matrix& matrix) {
  // Validate dimensions before accessing elements.
  if (matrix.Rows() != 2 || matrix.Cols() != 2) {
    throw std::invalid_argument("Eigendecomposition requires a 2x2 matrix");
  }

  const double a = matrix(0, 0);
  const double d = matrix(1, 1);

  // Check approximate symmetry.
  constexpr double symmetryTolerance = 1e-9;
  const double upper = matrix(0, 1);
  const double lower = matrix(1, 0);

  if (std::abs(upper - lower) > symmetryTolerance) {
    throw std::invalid_argument(
        "Eigendecomposition requires a symmetric matrix");
  }

  // Use the symmetric part of the input.
  const double b = (upper + lower) / 2.0;

  EigenResult result{std::vector<double>(2, 0.0), Matrix{2, 2}};

  // Calculate eigenvalues in descending order.
  const double gap = std::hypot(a - d, 2.0 * b);

  const double lambda1 = (a + d + gap) / 2.0;
  const double lambda2 = (a + d - gap) / 2.0;

  result.eigenvalues[0] = lambda1;
  result.eigenvalues[1] = lambda2;

  double x;
  double y;

  if (b == 0.0) {
    // Diagonal matrix: eigenvectors align with axes.
    if (a >= d) {
      x = 1.0;
      y = 0.0;
    } else {
      x = 0.0;
      y = 1.0;
    }
  } else {
    // Two equivalent candidates for the first eigenvector.
    const double ax = b;
    const double ay = lambda1 - a;

    const double bx = lambda1 - d;
    const double by = b;

    // Choose the candidate with the larger norm.
    if (std::hypot(ax, ay) >= std::hypot(bx, by)) {
      x = ax;
      y = ay;
    } else {
      x = bx;
      y = by;
    }

    // Normalize the selected eigenvector.
    const double length = std::hypot(x, y);
    x /= length;
    y /= length;
  }

  // Eigenvectors are stored as columns.
  result.eigenvectors(0, 0) = x;
  result.eigenvectors(1, 0) = y;

  // Second eigenvector is perpendicular to the first.
  result.eigenvectors(0, 1) = -y;
  result.eigenvectors(1, 1) = x;

  return result;
}
