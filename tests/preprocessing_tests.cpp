#include <gtest/gtest.h>

#include "preprocessing.hpp"

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