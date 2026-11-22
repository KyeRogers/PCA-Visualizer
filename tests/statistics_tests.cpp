#include <gtest/gtest.h>

#include "statistics.hpp"

// Verify feature means and centered values.
TEST(PreprocessingTests, CentersDatasetCorrectly) {
  Matrix data{3, 2};
  data(0, 0) = 2.0;
  data(0, 1) = 4.0;
  data(1, 0) = 4.0;
  data(1, 1) = 6.0;
  data(2, 0) = 6.0;
  data(2, 1) = 8.0;

  Matrix expected{3, 2};
  expected(0, 0) = -2.0;
  expected(0, 1) = -2.0;
  expected(1, 0) = 0.0;
  expected(1, 1) = 0.0;
  expected(2, 0) = 2.0;
  expected(2, 1) = 2.0;

  CenteringResult result = CenterData(data);

  EXPECT_EQ(result.centered, expected);
  ASSERT_EQ(result.means.size(), 2);
  EXPECT_DOUBLE_EQ(result.means[0], 4.0);
  EXPECT_DOUBLE_EQ(result.means[1], 6.0);
}

// Already-centered data should remain unchanged.
TEST(PreprocessingTests, PreservesAlreadyCenteredData) {
  Matrix data{3, 2};
  data(0, 0) = -2.0;
  data(0, 1) = -4.0;
  data(1, 0) = 0.0;
  data(1, 1) = 0.0;
  data(2, 0) = 2.0;
  data(2, 1) = 4.0;

  CenteringResult result = CenterData(data);

  EXPECT_EQ(result.centered, data);
  ASSERT_EQ(result.means.size(), 2);
  EXPECT_DOUBLE_EQ(result.means[0], 0.0);
  EXPECT_DOUBLE_EQ(result.means[1], 0.0);
}

// Centering must not modify the original dataset.
TEST(PreprocessingTests, PreservesOriginalDataset) {
  Matrix data{2, 2};
  data(0, 0) = 1.0;
  data(0, 1) = 3.0;
  data(1, 0) = 5.0;
  data(1, 1) = 7.0;

  Matrix original = data;

  CenteringResult result = CenterData(data);

  EXPECT_EQ(data, original);
}

// A single observation should center to zero.
TEST(PreprocessingTests, CentersSingleObservation) {
  Matrix data{1, 2};
  data(0, 0) = 5.0;
  data(0, 1) = -3.0;

  CenteringResult result = CenterData(data);

  Matrix expected{1, 2};

  EXPECT_EQ(result.centered, expected);
  ASSERT_EQ(result.means.size(), 2);
  EXPECT_DOUBLE_EQ(result.means[0], 5.0);
  EXPECT_DOUBLE_EQ(result.means[1], -3.0);
}

// Verify covariance for two positively correlated features.
TEST(CovarianceTests, ComputesKnownCovariance) {
  Matrix centered{3, 2};
  centered(0, 0) = -2.0;
  centered(0, 1) = -4.0;
  centered(1, 0) = 0.0;
  centered(1, 1) = 0.0;
  centered(2, 0) = 2.0;
  centered(2, 1) = 4.0;

  Matrix expected{2, 2};
  expected(0, 0) = 4.0;
  expected(0, 1) = 8.0;
  expected(1, 0) = 8.0;
  expected(1, 1) = 16.0;

  EXPECT_EQ(ComputeCovariance(centered), expected);
}

// Covariance matrices must be symmetric.
TEST(CovarianceTests, CovarianceIsSymmetric) {
  Matrix centered{3, 3};
  centered(0, 0) = -1.0;
  centered(0, 1) = 2.0;
  centered(0, 2) = 3.0;
  centered(1, 0) = 0.0;
  centered(1, 1) = -4.0;
  centered(1, 2) = -6.0;
  centered(2, 0) = 1.0;
  centered(2, 1) = 2.0;
  centered(2, 2) = 3.0;

  Matrix covariance = ComputeCovariance(centered);

  EXPECT_EQ(covariance, covariance.Transpose());
}

// A constant feature has zero variance and zero covariance.
TEST(CovarianceTests, ZeroVarianceFeature) {
  Matrix centered{3, 2};
  centered(0, 0) = -2.0;
  centered(0, 1) = 0.0;
  centered(1, 0) = 0.0;
  centered(1, 1) = 0.0;
  centered(2, 0) = 2.0;
  centered(2, 1) = 0.0;

  Matrix expected{2, 2};
  expected(0, 0) = 4.0;

  EXPECT_EQ(ComputeCovariance(centered), expected);
}

// Sample covariance requires at least two observations.
TEST(CovarianceTests, RejectsSingleObservation) {
  Matrix centered{1, 2};

  EXPECT_THROW(ComputeCovariance(centered), std::invalid_argument);
}

// A single feature produces a 1x1 covariance matrix containing its variance.
TEST(CovarianceTests, SingleFeature) {
  Matrix centered{3, 1};
  centered(0, 0) = -2.0;
  centered(1, 0) = 0.0;
  centered(2, 0) = 2.0;

  Matrix expected{1, 1};
  expected(0, 0) = 4.0;

  EXPECT_EQ(ComputeCovariance(centered), expected);
}

// Verification of the eigenvector and value calculation for a diagonal matrix
TEST(EigenvalueTests, DiagonalMatrix) {
  Matrix diagonal{2, 2};
  diagonal(0, 0) = 9.0;
  diagonal(1, 1) = 4.0;

  EigenResult result = EigenDecomposeSymmetric2x2(diagonal);

  // 1. Verify eigenvalue count and eigenvector dimensions.
  ASSERT_EQ(result.eigenvalues.size(), 2);
  ASSERT_EQ(result.eigenvectors.Rows(), 2);
  ASSERT_EQ(result.eigenvectors.Cols(), 2);
  // 2. Verify eigenvalues are approximately 9 and 4.
  EXPECT_NEAR(result.eigenvalues[0], 9.0, 1e-9);
  EXPECT_NEAR(result.eigenvalues[1], 4.0, 1e-9);
  // Verify Av = lambda*v and unit length.
  for (size_t i = 0; i < 2; ++i) {
    double x = result.eigenvectors(0, i);
    double y = result.eigenvectors(1, i);
    double lambda = result.eigenvalues[i];

    double transformedX = diagonal(0, 0) * x + diagonal(0, 1) * y;
    double transformedY = diagonal(1, 0) * x + diagonal(1, 1) * y;
    
    // Check x-component of Av = lambda*v.
    EXPECT_NEAR(transformedX, lambda * x, 1e-9);
    // Check y-component of Av = lambda*v.
    EXPECT_NEAR(transformedY, lambda * y, 1e-9);
    // Check x*x + y*y = 1.
    EXPECT_NEAR(x * x + y * y, 1.0, 1e-9);
  }

  // Verify perpendicularity.
  // Calculate the dot product of columns 0 and 1.
  // Check that it is approximately zero.
  double dot = result.eigenvectors(0, 0) * result.eigenvectors(0, 1) +
               result.eigenvectors(1, 0) * result.eigenvectors(1, 1);

  EXPECT_NEAR(dot, 0.0, 1e-9);
}


#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

// Assumes statistics.hpp provides Matrix, EigenResult,
// and EigenDecomposeSymmetric2x2().
#include "statistics.hpp"

// Reusable assertions for symmetric 2x2 eigendecomposition.
void ExpectValidEigenDecomposition(const Matrix& matrix,
                                   const EigenResult& result) {
  constexpr double tolerance = 1e-9;

  ASSERT_EQ(result.eigenvalues.size(), 2);
  ASSERT_EQ(result.eigenvectors.Rows(), 2);
  ASSERT_EQ(result.eigenvectors.Cols(), 2);

  // Eigenvalues should be sorted in descending order.
  EXPECT_GE(result.eigenvalues[0], result.eigenvalues[1]);

  for (size_t i = 0; i < 2; ++i) {
    double x = result.eigenvectors(0, i);
    double y = result.eigenvectors(1, i);
    double lambda = result.eigenvalues[i];

    // Check A*v = lambda*v.
    double transformedX = matrix(0, 0) * x + matrix(0, 1) * y;
    double transformedY = matrix(1, 0) * x + matrix(1, 1) * y;

    EXPECT_NEAR(transformedX, lambda * x, tolerance);
    EXPECT_NEAR(transformedY, lambda * y, tolerance);

    // Each eigenvector must have unit length.
    EXPECT_NEAR(x * x + y * y, 1.0, tolerance);
  }

  // Eigenvectors must be perpendicular.
  double dot =
      result.eigenvectors(0, 0) * result.eigenvectors(0, 1) +
      result.eigenvectors(1, 0) * result.eigenvectors(1, 1);

  EXPECT_NEAR(dot, 0.0, tolerance);
}

TEST(EigenvalueTests, RotatedSymmetricMatrix) {
  Matrix matrix{2, 2};
  matrix(0, 0) = 4.0;
  matrix(0, 1) = 2.0;
  matrix(1, 0) = 2.0;
  matrix(1, 1) = 4.0;

  EigenResult result = EigenDecomposeSymmetric2x2(matrix);

  EXPECT_NEAR(result.eigenvalues[0], 6.0, 1e-9);
  EXPECT_NEAR(result.eigenvalues[1], 2.0, 1e-9);

  ExpectValidEigenDecomposition(matrix, result);
}

TEST(EigenvalueTests, ReversedDiagonalMatrix) {
  Matrix matrix{2, 2};
  matrix(0, 0) = 4.0;
  matrix(1, 1) = 9.0;

  EigenResult result = EigenDecomposeSymmetric2x2(matrix);

  EXPECT_NEAR(result.eigenvalues[0], 9.0, 1e-9);
  EXPECT_NEAR(result.eigenvalues[1], 4.0, 1e-9);

  ExpectValidEigenDecomposition(matrix, result);
}

TEST(EigenvalueTests, RepeatedEigenvalues) {
  Matrix matrix{2, 2};
  matrix(0, 0) = 5.0;
  matrix(1, 1) = 5.0;

  EigenResult result = EigenDecomposeSymmetric2x2(matrix);

  EXPECT_NEAR(result.eigenvalues[0], 5.0, 1e-9);
  EXPECT_NEAR(result.eigenvalues[1], 5.0, 1e-9);

  ExpectValidEigenDecomposition(matrix, result);
}

TEST(EigenvalueTests, NegativeCovariance) {
  Matrix matrix{2, 2};
  matrix(0, 0) = 4.0;
  matrix(0, 1) = -2.0;
  matrix(1, 0) = -2.0;
  matrix(1, 1) = 4.0;

  EigenResult result = EigenDecomposeSymmetric2x2(matrix);

  EXPECT_NEAR(result.eigenvalues[0], 6.0, 1e-9);
  EXPECT_NEAR(result.eigenvalues[1], 2.0, 1e-9);

  ExpectValidEigenDecomposition(matrix, result);
}

TEST(EigenvalueTests, RejectsInvalidInput) {
  Matrix wrongDimensions{3, 2};

  EXPECT_THROW(EigenDecomposeSymmetric2x2(wrongDimensions),
               std::invalid_argument);

  Matrix nonSymmetric{2, 2};
  nonSymmetric(0, 1) = 2.0;
  nonSymmetric(1, 0) = 3.0;

  EXPECT_THROW(EigenDecomposeSymmetric2x2(nonSymmetric),
               std::invalid_argument);
}
