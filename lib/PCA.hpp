#ifndef PCA_HPP
#define PCA_HPP

#include "matrix.hpp"

class PCA {
 public:
  PCA(); 

  std::vector<double> Means() const;
  std::vector<double> Eigenvalues() const;
  Matrix Eigenvectors() const;
  bool IsFitted() const;

  void Fit(const Matrix& data);
  Matrix Transform(const Matrix& data, size_t components) const;
  Matrix InverseTransform(const Matrix& projectedData) const;

 private:
  std::vector<double> means_;
  std::vector<double> eigenvalues_;
  Matrix eigenvectors_;
  bool fitted_ = false;
};

#endif