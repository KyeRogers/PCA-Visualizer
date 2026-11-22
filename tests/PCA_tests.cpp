
#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"
#include "PCA.hpp"

// A simple dataset with greater variance along X than Y.
static Matrix MakeTestDataset() {
    Matrix data(4, 2);

    data(0, 0) = 1.0;
    data(0, 1) = 2.0;

    data(1, 0) = 3.0;
    data(1, 1) = 2.0;

    data(2, 0) = 5.0;
    data(2, 1) = 2.0;

    data(3, 0) = 7.0;
    data(3, 1) = 2.0;

    return data;
}

// Fit should mark the model as fitted.
TEST(PCAFitTests, MarksModelAsFitted) {
    PCA pca;

    EXPECT_FALSE(pca.IsFitted());

    Matrix data = MakeTestDataset();
    pca.Fit(data);

    EXPECT_TRUE(pca.IsFitted());
}

// Fit should calculate and store the feature means.
TEST(PCAFitTests, CalculatesFeatureMeans) {
    PCA pca;
    pca.Fit(MakeTestDataset());

    const auto& means = pca.Means();

    ASSERT_EQ(means.size(), 2);

    EXPECT_NEAR(means[0], 4.0, 1e-9);
    EXPECT_NEAR(means[1], 2.0, 1e-9);
}

// Fit should calculate eigenvalues from the covariance matrix.
// The dataset varies only along X, so PC2 has zero variance.
TEST(PCAFitTests, CalculatesEigenvalues) {
    PCA pca;
    pca.Fit(MakeTestDataset());

    const auto& eigenvalues = pca.Eigenvalues();

    ASSERT_EQ(eigenvalues.size(), 2);

    EXPECT_NEAR(eigenvalues[0], 20.0 / 3.0, 1e-9);
    EXPECT_NEAR(eigenvalues[1], 0.0, 1e-9);
}

// Fit should identify the X axis as PC1.
TEST(PCAFitTests, CalculatesPrincipalDirections) {
    PCA pca;
    pca.Fit(MakeTestDataset());

    const Matrix& eigenvectors = pca.Eigenvectors();

    ASSERT_EQ(eigenvectors.Rows(), 2);
    ASSERT_EQ(eigenvectors.Cols(), 2);

    // Eigenvector signs are arbitrary, so check absolute values.
    EXPECT_NEAR(std::abs(eigenvectors(0, 0)), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(eigenvectors(1, 0)), 0.0, 1e-9);

    EXPECT_NEAR(std::abs(eigenvectors(0, 1)), 0.0, 1e-9);
    EXPECT_NEAR(std::abs(eigenvectors(1, 1)), 1.0, 1e-9);
}

// The current implementation only supports 2D data.
TEST(PCAFitTests, RejectsIncorrectFeatureCount) {
    PCA pca;
    Matrix data(5, 3);

    EXPECT_THROW(pca.Fit(data), std::invalid_argument);
    EXPECT_FALSE(pca.IsFitted());
}

// Sample covariance requires at least two observations.
TEST(PCAFitTests, RejectsSingleObservation) {
    PCA pca;
    Matrix data(1, 2);

    EXPECT_THROW(pca.Fit(data), std::invalid_argument);
    EXPECT_FALSE(pca.IsFitted());
}

// Re-fitting should replace the parameters learned previously.
TEST(PCAFitTests, RefittingUpdatesLearnedParameters) {
    PCA pca;
    pca.Fit(MakeTestDataset());

    Matrix secondData(3, 2);

    secondData(0, 0) = 10.0;
    secondData(0, 1) = 20.0;

    secondData(1, 0) = 12.0;
    secondData(1, 1) = 20.0;

    secondData(2, 0) = 14.0;
    secondData(2, 1) = 20.0;

    pca.Fit(secondData);

    ASSERT_EQ(pca.Means().size(), 2);

    EXPECT_NEAR(pca.Means()[0], 12.0, 1e-9);
    EXPECT_NEAR(pca.Means()[1], 20.0, 1e-9);

    EXPECT_NEAR(pca.Eigenvalues()[0], 4.0, 1e-9);
    EXPECT_NEAR(pca.Eigenvalues()[1], 0.0, 1e-9);
}

class PCATransformTests : public ::testing::Test {
protected:
    PCA pca;

    void SetUp() override {
        Matrix data(4, 2);

        data(0, 0) = 1.0;
        data(0, 1) = 2.0;

        data(1, 0) = 3.0;
        data(1, 1) = 2.0;

        data(2, 0) = 5.0;
        data(2, 1) = 2.0;

        data(3, 0) = 7.0;
        data(3, 1) = 2.0;

        pca.Fit(data);
    }
};

// Transforming with both components preserves the number
// of observations and features.
TEST_F(PCATransformTests, TransformWithTwoComponents) {
    Matrix data(2, 2);

    data(0, 0) = 5.0;
    data(0, 1) = 2.0;

    data(1, 0) = 7.0;
    data(1, 1) = 2.0;

    Matrix result = pca.Transform(data, 2);

    ASSERT_EQ(result.Rows(), 2);
    ASSERT_EQ(result.Cols(), 2);

    // Eigenvector signs are arbitrary.
    EXPECT_NEAR(std::abs(result(0, 0)), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(result(1, 0)), 3.0, 1e-9);

    EXPECT_NEAR(result(0, 1), 0.0, 1e-9);
    EXPECT_NEAR(result(1, 1), 0.0, 1e-9);
}

// Keeping only PC1 reduces the data from 2D to 1D.
TEST_F(PCATransformTests, TransformWithOneComponent) {
    Matrix data(2, 2);

    data(0, 0) = 5.0;
    data(0, 1) = 2.0;

    data(1, 0) = 7.0;
    data(1, 1) = 2.0;

    Matrix result = pca.Transform(data, 1);

    ASSERT_EQ(result.Rows(), 2);
    ASSERT_EQ(result.Cols(), 1);

    EXPECT_NEAR(std::abs(result(0, 0)), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(result(1, 0)), 3.0, 1e-9);
}

// Transform must use the means learned during Fit,
// not recalculate means from the new dataset.
TEST_F(PCATransformTests, UsesTrainingMeans) {
    Matrix newData(1, 2);

    newData(0, 0) = 10.0;
    newData(0, 1) = 2.0;

    Matrix result = pca.Transform(newData, 1);

    ASSERT_EQ(result.Rows(), 1);
    ASSERT_EQ(result.Cols(), 1);

    // Training mean X = 4, so centered X = 6.
    EXPECT_NEAR(std::abs(result(0, 0)), 6.0, 1e-9);
}

// Invalid component counts must be rejected.
TEST_F(PCATransformTests, RejectsInvalidComponentCounts) {
    Matrix data(2, 2);

    EXPECT_THROW(pca.Transform(data, 0), std::invalid_argument);
    EXPECT_THROW(pca.Transform(data, 3), std::invalid_argument);
}

// A fitted 2D PCA model cannot transform 3D observations.
TEST_F(PCATransformTests, RejectsIncorrectFeatureCount) {
    Matrix data(3, 3);

    EXPECT_THROW(pca.Transform(data, 1), std::invalid_argument);
}

// Transform cannot be used before fitting.
TEST(PCATransformStateTests, RejectsTransformBeforeFit) {
    PCA pca;
    Matrix data(2, 2);

    EXPECT_THROW(pca.Transform(data, 1), std::logic_error);
}

// InverseTransform with both components should recover
// the original observations.
TEST_F(PCATransformTests, FullReconstructionRecoversOriginalData) {
    Matrix original(3, 2);

    original(0, 0) = 2.0;
    original(0, 1) = 4.0;

    original(1, 0) = 5.0;
    original(1, 1) = 8.0;

    original(2, 0) = 9.0;
    original(2, 1) = 1.0;

    Matrix projected = pca.Transform(original, 2);
    Matrix reconstructed = pca.InverseTransform(projected);

    ASSERT_EQ(reconstructed.Rows(), original.Rows());
    ASSERT_EQ(reconstructed.Cols(), original.Cols());

    for (size_t row = 0; row < original.Rows(); row++) {
        for (size_t col = 0; col < original.Cols(); col++) {
            EXPECT_NEAR(
                reconstructed(row, col),
                original(row, col),
                1e-9
            );
        }
    }
}

// With one component, reconstruction loses information
// along the discarded principal direction.
TEST_F(PCATransformTests, OneComponentReconstruction) {
    Matrix original(2, 2);

    original(0, 0) = 5.0;
    original(0, 1) = 10.0;

    original(1, 0) = 7.0;
    original(1, 1) = -4.0;

    Matrix projected = pca.Transform(original, 1);
    Matrix reconstructed = pca.InverseTransform(projected);

    ASSERT_EQ(reconstructed.Rows(), 2);
    ASSERT_EQ(reconstructed.Cols(), 2);

    // PC1 is horizontal, so X is preserved.
    // Y is reconstructed as the training mean (2).
    EXPECT_NEAR(reconstructed(0, 0), 5.0, 1e-9);
    EXPECT_NEAR(reconstructed(0, 1), 2.0, 1e-9);

    EXPECT_NEAR(reconstructed(1, 0), 7.0, 1e-9);
    EXPECT_NEAR(reconstructed(1, 1), 2.0, 1e-9);
}

// InverseTransform should reject projected data with
// more components than the fitted model supports.
TEST_F(PCATransformTests, RejectsTooManyProjectedComponents) {
    Matrix projected(3, 3);

    EXPECT_THROW(
        pca.InverseTransform(projected),
        std::invalid_argument
    );
}

// InverseTransform cannot be used before fitting.
TEST(PCATransformStateTests, RejectsInverseTransformBeforeFit) {
    PCA pca;
    Matrix projected(2, 1);

    EXPECT_THROW(
        pca.InverseTransform(projected),
        std::logic_error
    );
}
