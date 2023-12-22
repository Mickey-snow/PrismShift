#include<gtest/gtest.h>

#include "util/matrix.hpp"


TEST(MatrixTest, RREF){
  // Test for correctness with a general matrix
  {
    Matrix<3,6> mat{
      0,3,-6,6,4,-5,
      3,-7,8,-5,8,9,
      3,-9,12,-9,6,15};

    Matrix<3,6> reduced{
      1,0,-2,3,0,-24,
      0,1,-2,2,0,-7,
      0,0,0,0,1,4
    };
    EXPECT_EQ(mat.rref(), reduced);
  }
  
  {
    Matrix<3,3> mat{1,2,3,4,5,6,7,8,9};
    Matrix<3,3> reduced{1,0,-1,0,1,2,0,0,0};
    EXPECT_EQ(mat.rref(), reduced);
  }

  // Test for a matrix that is already in rref
  {
    Matrix<3,6> mat{
      1,0,-2,3,0,-24,
      0,1,-2,2,0,-7,
      0,0,0,0,1,4
    };
    auto reduced = mat;
    EXPECT_EQ(mat.rref(), reduced);
  }
  {
    Matrix<3,3> mat{1,0,0,0,0,1,0,0,0};
    auto reduced = mat;
    EXPECT_EQ(mat.rref(), reduced);
  }

  // Test with a zero matrix
  {
    Matrix<2,2> mat{0,0,0,0};
    auto reduced = mat;
    EXPECT_EQ(mat.rref(), reduced);
  }
  
  // Test with a single row/column matrix
  {
    Matrix<1,3> mat{2,4,8};
    Matrix<1,3> reduced{1,2,4};
    EXPECT_EQ(mat.rref(), reduced);
  }
  {
    Matrix<3,1> mat{1,5,9};
    Matrix<3,1> reduced{1,0,0};
    EXPECT_EQ(mat.rref(), reduced);
  }
}


TEST(MatrixTest, Transpose){
  {
    Matrix<2,2> mat{1, 2, 3, 4};
    Matrix<2,2> expected{1, 3, 2, 4};
    EXPECT_EQ(mat.T(), expected);
  }

  // Test transposing a rectangular matrix
  {
    Matrix<2,3> mat{1, 2, 3, 4, 5, 6};
    Matrix<3,2> expected{1, 4, 2, 5, 3, 6};
    EXPECT_EQ(mat.T(), expected);
  }

  // Test transposing a matrix with one row
  {
    Matrix<1,3> mat{1, 2, 3};
    Matrix<3,1> expected{1, 2, 3};
    EXPECT_EQ(mat.T(), expected);
  }

  // Test transposing a matrix with one column
  {
    Matrix<3,1> mat{1, 2, 3};
    Matrix<1,3> expected{1, 2, 3};
    EXPECT_EQ(mat.T(), expected);
  }

  // Test transposing a 1x1 matrix
  {
    Matrix<1,1> mat{1};
    Matrix<1,1> expected{1};
    EXPECT_EQ(mat.T(), expected);
  }

  // Test transposing an empty matrix
  {
    Matrix<50,0> mat{};
    Matrix<0,50> expected{};
    EXPECT_EQ(mat.T(), expected);
  }
}

TEST(MatrixTest, AppendMatrix){
  // Test basic functionality
  {
    Matrix<3,3> mat1{1,2,3,4,5,6,7,8,9};
    Matrix<3,2> mat2{-1,-2,-3,-4,-5,-6};
    Matrix<3,5> expected{
      1,2,3,-1,-2,
      4,5,6,-3,-4,
      7,8,9,-5,-6};
    EXPECT_EQ(mat1.AppendRight(mat2), expected);
  }
  {
    Matrix<2,3> mat1{1,2,3,4,5,6};
    Matrix<1,3> mat2{7,8,9};
    Matrix<3,3> expected{1,2,3,4,5,6,7,8,9};
    EXPECT_EQ(mat1.AppendBottom(mat2), expected);
  }

  // Test appending empty matrix
  {
    Matrix<2,2> mat1{1,0,0,1};
    Matrix<2,0> mat2;
    EXPECT_EQ(mat1.AppendRight(mat2), mat1);

    Matrix<0,2> mat3;
    EXPECT_EQ(mat1.AppendBottom(mat3), mat1);
  }
}

TEST(MatrixTest, Inverse){
  // Test for basic functionality
  {
    Matrix<2,2> mat{1,2,3,4};
    Matrix<2,2> expected{
      -2,1,
      3.0/2,-1.0/2};
    EXPECT_EQ(mat.inv(), expected);
  }
}
