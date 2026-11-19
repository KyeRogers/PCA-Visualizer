#include <gtest/gtest.h>

#include <stdexcept>

#include "matrix.hpp"

class MatrixTests : public ::testing::Test {
 protected:
  Matrix matrix{2, 2};
};

// Check that the matrix initializes with the correct dimensions
TEST_F(MatrixTests, InitializesDimensionsCorrectly) {
  EXPECT_EQ(matrix.Rows(), 2);
  EXPECT_EQ(matrix.Cols(), 2);
}

// Checks that the () and element access is correctly overloaded
TEST_F(MatrixTests, ReadAndWriteElements) {

  matrix(0, 0) = 5.0;
  matrix(0, 1) = 3.0;

  // checks assignent and reading work correctly
  EXPECT_EQ(matrix(0, 1), 3.0);
  EXPECT_EQ(matrix(0, 0), 5.0);
}

TEST_F(MatrixTests, InvalidPositionCheck) {
  // checks that an error is thrown when accesing invalid positions
  EXPECT_THROW(matrix(0, 2), std::out_of_range);
  EXPECT_THROW(matrix(2, 0), std::out_of_range);
}

// Check that on initialize, the matrix elements are all 0
TEST_F(MatrixTests, InitializesToZero) {
  EXPECT_EQ(matrix(0, 0), 0.0);
  EXPECT_EQ(matrix(0, 1), 0.0);
}