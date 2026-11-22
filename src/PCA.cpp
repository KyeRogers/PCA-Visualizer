#include "PCA.hpp"

#include <stdexcept>

#include "statistics.hpp"

PCA::PCA() : means_(0), eigenvalues_(0), eigenvectors_{1, 1}, fitted_{false} {}

std::vector<double> PCA::Means() const { return means_; }
std::vector<double> PCA::Eigenvalues() const { return eigenvalues_; }
Matrix PCA::Eigenvectors() const { return eigenvectors_; }
bool PCA::IsFitted() const { return fitted_; }

void PCA::Fit(const Matrix& data) {
  // perform validations
  if (data.Rows() < 2 || data.Cols() > 2) {
    throw std::invalid_argument("Invalid input data for pca");
  }

  // calculate means and center data
  CenteringResult centering_result = CenterData(data);
  means_ = centering_result.means;

  // Calculate covariance matrix
  Matrix covariance_matrix = ComputeCovariance(centering_result.centered);

  // Calculate eigenvalues and vectors
  EigenResult eigen_result = EigenDecomposeSymmetric2x2(covariance_matrix);

  eigenvalues_ = eigen_result.eigenvalues;
  eigenvectors_ = eigen_result.eigenvectors;

  fitted_ = true;
}
Matrix PCA::Transform(const Matrix& data, size_t components) const {
  if (components < 1 || components > 2) {
    throw std::invalid_argument(
        "Cannot transform data to less than 1 or more than 2 componets");
  }
  if (!fitted_) {
    throw std::logic_error("Must call fit before transform");
  }
  if (data.Cols() != 2) {
    throw std::invalid_argument("Cannot transform data that doesnt have 2");
  }

  Matrix centered{static_cast<int>(data.Rows()), static_cast<int>(data.Cols())};
  for (size_t col{0}; col < centered.Cols(); col++) {
    for (size_t row{0}; row < centered.Rows(); row++) {
      centered(row, col) = data(row, col) - means_[col];
    }
  }

  if (components == 1) {
    Matrix eigenvector_temp{static_cast<int>(eigenvectors_.Rows()), 1};
    for (size_t row{0}; row < eigenvectors_.Rows(); row++) {
      eigenvector_temp(row, 0) = eigenvectors_(row, 0);
    }
    return ProjectData(centered, eigenvector_temp);
  }
  return ProjectData(centered, eigenvectors_);
}
Matrix PCA::InverseTransform(const Matrix& projectedData) const {
  if (!fitted_) {
    throw std::logic_error("Must call fit before inverse transform");
  }

    if (projectedData.Cols() == 1) {
    Matrix eigenvector_temp{static_cast<int>(eigenvectors_.Rows()), 1};
    for (size_t row{0}; row < eigenvectors_.Rows(); row++) {
      eigenvector_temp(row, 0) = eigenvectors_(row, 0);
    }
    return  ReconstructData(projectedData, eigenvector_temp, means_);
  }
  return ReconstructData(projectedData, eigenvectors_, means_);
}
