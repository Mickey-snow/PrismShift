#include<gtest/gtest.h>

#include "util/matrix.hpp"
#include "util/geometry.hpp"

TEST(MatrixTest, MultVector){
  // correctness
  {
    Matrix<3,2> mat{1,2,3,4,5,6};
    Vector2 v{2,1};
    EXPECT_EQ(mat*v, Vector3(4,10,16));
  }
  {
    Matrix<2,3> mat{2,3,4,1,0,-1};
    Vector3 v{1,2,3};
    EXPECT_EQ(mat*v, Vector2(20,-2));
  }
  {
    Matrix<2,2> mat{100,200,300,400};
    Vector2 v{10,20};
    EXPECT_EQ(mat*v, Vector2(5000,11000));
  }

  // identity matrix
  {
    Matrix<2,2> mat = Matrix<2,2>::I();
    Vector2 v{5,-31};
    EXPECT_EQ(mat*v, v);
  }

  // zero matrix
  {
    Matrix<3,3> mat;
    Vector3 v{-12,3,9.21};
    EXPECT_EQ(mat*v, Vector3(0,0,0));
  }
}

TEST(MatrixTest, MultMat){
  // standard
  {
    Matrix<2,2> A{1,2,3,4},B{2,0,1,3};
    Matrix<2,2> expected{4,6,10,12};
    EXPECT_EQ(A*B, expected);
  }
  {
    Matrix<2,2> A{100,200,300,400}, B{10,20,30,40};
    Matrix<2,2> expected{7000,10000,15000,22000};
    EXPECT_EQ(A*B, expected);
  }

  // non-square
  {
    Matrix<3,2> A{2,3,4,5,6,7};
    Matrix<2,3> B{1,2,3,4,5,6};
    Matrix<3,3> expected{14,19,24,24,33,42,34,47,60};
    EXPECT_EQ(A*B, expected);
  }

  // identity
  {
    Matrix<3,3> A{1,2,3,4,5,6,7,8,9}, id=Matrix<3,3>::I();
    EXPECT_EQ(A*id, A);
    EXPECT_EQ(id*A, A);
  }

  // zero matrix
  {
    Matrix<2,2> zero, A{1,2,3,4};
    EXPECT_EQ(zero*A, zero);
    EXPECT_EQ(A*zero, zero);
  }
}

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
  {
    Matrix<2,2> mat{4,7,2,6};
    Matrix<2,2> expected{0.6,-0.7,-0.2,0.4};
    EXPECT_EQ(mat.inv(), expected);
  }
  {
    Matrix<3,3> mat{3,0,2,2,0,-2,0,1,1};
    Matrix<3,3> expected{0.2,0.2,0,-0.2,0.3,1,0.2,-0.3,0};
    EXPECT_EQ(mat.inv(), expected);
  }
  {
    Matrix<4,4> mat{0.794645,0.613787,0.0587482,0.493943,0.222896,0.882244,0.828312,0.504613,0.44324,0.945123,0.774638,0.65932,0.434762,0.8777,0.855092,0.253013};
    EXPECT_NO_THROW(mat.inv());
  }
  
  // Test with identity matrix
  {
    Matrix<23,23> mat = Matrix<23,23>::I();
    EXPECT_EQ(mat.inv(), mat);
    // inverse of an identity matrix should be the identity matrix itself
  }

  // Test with non-invertable matrix
  {
    Matrix<3,3> zeromat;
    EXPECT_ANY_THROW(zeromat.inv());
  }
  {
    Matrix<2,2> mat{1,2,2,4};
    EXPECT_ANY_THROW(mat.inv());
  }
  {
    Matrix<1,3> notSquaremat{1,2,3};
    EXPECT_ANY_THROW(notSquaremat.inv());
  }

  // Test special cases
  {
    Matrix<2,2> symmetricMat{1,2,2,3};
    Matrix<2,2> expected{-3,2,2,-1};
    EXPECT_EQ(symmetricMat.inv(), expected);
  }
  {
    Matrix<2,2> diagonalMat{5,0,0,4};
    Matrix<2,2> expected{1.0/5,0,0,1.0/4};
    EXPECT_EQ(diagonalMat.inv(), expected);
  }
}


TEST(MatrixTest, Det){
  // basic functionality
  {
    Matrix<2,2> mat{1,2,3,4};
    EXPECT_DOUBLE_EQ(mat.det(), -2.0);
  }
  {
    Matrix<3,3> mat{2, 1, 3, 1, 0, 1, 3, 2, 0};
    EXPECT_DOUBLE_EQ(mat.det(), 5.0);
  }

  // zero matrix
  {
    Matrix<5,5> mat;
    EXPECT_DOUBLE_EQ(mat.det(), 0.0);
  }

  // upper / lower triangular matrix
  {
    Matrix<3,3> upperTriMat{1,2,3,0,4,5,0,0,9};
    EXPECT_DOUBLE_EQ(upperTriMat.det(), 36.0);
    Matrix<4,4> lowerTriMat{0.1,0,0,0,7,17,0,0,9,-5.2,23,0,7000,-1,1.5,-92.3};
    EXPECT_DOUBLE_EQ(lowerTriMat.det(), -3608.93);
  }

  // identity matrix
  {
    auto idMat = Matrix<63,63>::I();
    EXPECT_DOUBLE_EQ(idMat.det(), 1.0);
  }

  // non-square matrix
  {
    Matrix<2,3> mat;
    EXPECT_ANY_THROW(mat.det());
  }
}
