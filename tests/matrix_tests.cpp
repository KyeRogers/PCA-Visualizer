#include <gtest/gtest.h>

#include <stdexcept>

#include "matrix.hpp"

class MatrixTests : public ::testing::Test {
 protected:
  Matrix matrix{2, 2};
};

// checks ony valid matrix sizes
TEST(MatrixConstructionTests, RejectsInvalidDimensions) {
  // checks invalid dimensions
  EXPECT_THROW(Matrix matrix(0, 1), std::invalid_argument);
  EXPECT_THROW(Matrix matrix(1, 0), std::invalid_argument);
  EXPECT_THROW(Matrix matrix(-1, 1), std::invalid_argument);
  EXPECT_THROW(Matrix matrix(1, -1), std::invalid_argument);
}

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

TEST(MatrixOverloadTests, CheckEqualsComparison) {

  Matrix matrix{1, 2};
  matrix(0, 0) = 1.0;
  matrix(0, 1) = 2.0;

  Matrix matrix2{1, 2};
  matrix2(0, 0) = 1.0;
  matrix2(0, 1) = 2.0;
  // check theyre equal
  EXPECT_TRUE(matrix == matrix2);

  // check one value differnece akes them not equal
  matrix2(0, 1) = 3.0;
  EXPECT_FALSE(matrix == matrix2);

  // same values but differnt dimensions -> not equal
  Matrix matrix3{2, 1};
  matrix3(0, 0) = 1.0;
  matrix3(1, 0) = 2.0;

  EXPECT_FALSE(matrix == matrix3);

  Matrix matrix4{4, 4};
  Matrix matrix5{4, 4};
  EXPECT_TRUE(matrix4 == matrix5);

}

// Verify transposition of a rectangular 2x3 matrix
TEST(MatrixTransposeTests, TransposeRectangular) {
    Matrix matrix{2, 3};
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0;
    matrix(1, 1) = 5.0;
    matrix(1, 2) = 6.0;

    Matrix expected{3, 2};
    expected(0, 0) = 1.0;
    expected(0, 1) = 4.0;
    expected(1, 0) = 2.0;
    expected(1, 1) = 5.0;
    expected(2, 0) = 3.0;
    expected(2, 1) = 6.0;

    EXPECT_EQ(matrix.Transpose(), expected);
}

// Verify transposition of a square 3x3 matrix
TEST(MatrixTransposeTests, TransposeSquare) {
    Matrix matrix{3, 3};
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0;
    matrix(1, 1) = 5.0;
    matrix(1, 2) = 6.0;
    matrix(2, 0) = 7.0;
    matrix(2, 1) = 8.0;
    matrix(2, 2) = 9.0;

    Matrix expected{3, 3};
    expected(0, 0) = 1.0;
    expected(0, 1) = 4.0;
    expected(0, 2) = 7.0;
    expected(1, 0) = 2.0;
    expected(1, 1) = 5.0;
    expected(1, 2) = 8.0;
    expected(2, 0) = 3.0;
    expected(2, 1) = 6.0;
    expected(2, 2) = 9.0;

    EXPECT_EQ(matrix.Transpose(), expected);
}

// Verify a row vector becomes a column vector
TEST(MatrixTransposeTests, TransposeRowVector) {
    Matrix matrix{1, 4};
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(0, 3) = 4.0;

    Matrix expected{4, 1};
    expected(0, 0) = 1.0;
    expected(1, 0) = 2.0;
    expected(2, 0) = 3.0;
    expected(3, 0) = 4.0;

    EXPECT_EQ(matrix.Transpose(), expected);
}

// Verify that transposing twice restores the original matrix
TEST(MatrixTransposeTests, TransposeTwiceRestoresOriginal) {
    Matrix matrix{2, 3};
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0;
    matrix(1, 1) = 5.0;
    matrix(1, 2) = 6.0;

    EXPECT_EQ(matrix.Transpose().Transpose(), matrix);
}

// Verify transposition does not modify the original matrix
TEST(MatrixTransposeTests, TransposePreservesOriginal) {
    Matrix matrix{2, 3};
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0;
    matrix(1, 1) = 5.0;
    matrix(1, 2) = 6.0;

    Matrix original = matrix;

    Matrix transposed = matrix.Transpose();

    EXPECT_EQ(matrix, original);
    EXPECT_EQ(transposed.Rows(), 3);
    EXPECT_EQ(transposed.Cols(), 2);
}


// 2x3 multiplied by 3x2 produces a 2x2 matrix.
TEST(MatrixMultiplicationTests, MultiplyRectangular) {
    Matrix A{2, 3};
    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(0, 2) = 3.0;
    A(1, 0) = 4.0;
    A(1, 1) = 5.0;
    A(1, 2) = 6.0;

    Matrix B{3, 2};
    B(0, 0) = 7.0;
    B(0, 1) = 8.0;
    B(1, 0) = 9.0;
    B(1, 1) = 10.0;
    B(2, 0) = 11.0;
    B(2, 1) = 12.0;

    Matrix expected{2, 2};
    expected(0, 0) = 58.0;
    expected(0, 1) = 64.0;
    expected(1, 0) = 139.0;
    expected(1, 1) = 154.0;

    EXPECT_EQ(A * B, expected);
}

// Multiplication by the identity matrix preserves the matrix.
TEST(MatrixMultiplicationTests, MultiplyByIdentity) {
    Matrix A{2, 3};
    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(0, 2) = 3.0;
    A(1, 0) = 4.0;
    A(1, 1) = 5.0;
    A(1, 2) = 6.0;

    Matrix identity{3, 3};
    identity(0, 0) = 1.0;
    identity(1, 1) = 1.0;
    identity(2, 2) = 1.0;

    EXPECT_EQ(A * identity, A);
}

// Multiplication by a zero matrix produces a zero matrix.
TEST(MatrixMultiplicationTests, MultiplyByZero) {
    Matrix A{2, 3};
    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(1, 2) = 3.0;

    Matrix zero{3, 4};
    Matrix expected{2, 4};

    EXPECT_EQ(A * zero, expected);
}

// Matrix multiplication is generally not commutative.
TEST(MatrixMultiplicationTests, MultiplicationIsNotCommutative) {
    Matrix A{2, 2};
    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(1, 0) = 3.0;
    A(1, 1) = 4.0;

    Matrix B{2, 2};
    B(0, 0) = 0.0;
    B(0, 1) = 1.0;
    B(1, 0) = 1.0;
    B(1, 1) = 0.0;

    EXPECT_FALSE(A * B == B * A);
}

// Incompatible dimensions must be rejected.
TEST(MatrixMultiplicationTests, RejectIncompatibleDimensions) {
    Matrix A{2, 3};
    Matrix B{2, 4};

    EXPECT_THROW(A * B, std::invalid_argument);
}

// Multiplication should not modify either operand.
TEST(MatrixMultiplicationTests, PreservesOperands) {
    Matrix A{2, 2};
    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(1, 0) = 3.0;
    A(1, 1) = 4.0;

    Matrix B{2, 2};
    B(0, 0) = 5.0;
    B(0, 1) = 6.0;
    B(1, 0) = 7.0;
    B(1, 1) = 8.0;

    Matrix originalA = A;
    Matrix originalB = B;

    Matrix result = A * B;

    EXPECT_EQ(A, originalA);
    EXPECT_EQ(B, originalB);
}

TEST(MatrixAdditionTests, AddsElementsCorrectly) {
    Matrix A{2, 2};
    A(0, 0) = 1.0;
    A(0, 1) = -2.0;
    A(1, 0) = 3.0;
    A(1, 1) = 4.0;

    Matrix B{2, 2};
    B(0, 0) = 5.0;
    B(0, 1) = 6.0;
    B(1, 0) = -3.0;
    B(1, 1) = 2.0;

    Matrix expected{2, 2};
    expected(0, 0) = 6.0;
    expected(0, 1) = 4.0;
    expected(1, 0) = 0.0;
    expected(1, 1) = 6.0;

    EXPECT_EQ(A + B, expected);
}

TEST(MatrixAdditionTests, AddingZeroPreservesMatrix) {
    Matrix A{2, 3};
    A(0, 0) = 1.0;
    A(1, 2) = -4.0;

    Matrix zero{2, 3};

    EXPECT_EQ(A + zero, A);
}

TEST(MatrixAdditionTests, RejectsDifferentDimensions) {
    Matrix A{2, 3};
    Matrix B{3, 2};

    EXPECT_THROW(A + B, std::invalid_argument);
}

TEST(MatrixSubtractionTests, SubtractsElementsCorrectly) {
    Matrix A{2, 2};
    A(0, 0) = 5.0;
    A(0, 1) = 3.0;
    A(1, 0) = -2.0;
    A(1, 1) = 8.0;

    Matrix B{2, 2};
    B(0, 0) = 2.0;
    B(0, 1) = 5.0;
    B(1, 0) = 4.0;
    B(1, 1) = 3.0;

    Matrix expected{2, 2};
    expected(0, 0) = 3.0;
    expected(0, 1) = -2.0;
    expected(1, 0) = -6.0;
    expected(1, 1) = 5.0;

    EXPECT_EQ(A - B, expected);
}

TEST(MatrixSubtractionTests, SubtractingSelfProducesZero) {
    Matrix A{2, 3};
    A(0, 0) = 4.0;
    A(0, 2) = -3.0;
    A(1, 1) = 7.0;

    Matrix zero{2, 3};

    EXPECT_EQ(A - A, zero);
}

TEST(MatrixSubtractionTests, RejectsDifferentDimensions) {
    Matrix A{2, 3};
    Matrix B{2, 4};

    EXPECT_THROW(A - B, std::invalid_argument);
}

TEST(MatrixScalarTests, MultipliesEveryElement) {
    Matrix A{2, 2};
    A(0, 0) = 1.0;
    A(0, 1) = -2.0;
    A(1, 0) = 3.0;
    A(1, 1) = 4.0;

    Matrix expected{2, 2};
    expected(0, 0) = 2.5;
    expected(0, 1) = -5.0;
    expected(1, 0) = 7.5;
    expected(1, 1) = 10.0;

    EXPECT_EQ(A * 2.5, expected);
}

TEST(MatrixScalarTests, MultiplicationByZeroProducesZero) {
    Matrix A{2, 3};
    A(0, 0) = 3.0;
    A(1, 2) = -5.0;

    Matrix zero{2, 3};

    EXPECT_EQ(A * 0.0, zero);
}

TEST(MatrixScalarTests, MultiplicationByOnePreservesMatrix) {
    Matrix A{2, 3};
    A(0, 0) = 3.0;
    A(1, 2) = -5.0;

    EXPECT_EQ(A * 1.0, A);
}