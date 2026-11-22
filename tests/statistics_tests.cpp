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