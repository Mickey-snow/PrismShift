#include<gtest/gtest.h>
#include "constant.hpp"

#include<transform.hpp>
#include<matrix.hpp>
#include<random.hpp>

#include<cmath>

TEST(matrix, product){
  Matrix4 I = Matrix4::I();
  Matrix4 mat;
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j)
      mat[i][j] = i+j*i+4;

  EXPECT_EQ(mat, mat*I);
  EXPECT_EQ(mat, I*mat);
}

TEST(matrix, determinant){
  Matrix4 m{5,-7,2,2,0,3,0,-4,-5,-8,0,3,0,5,0,-6};
  EXPECT_NEAR(m.Det(),20,EPS);
}

TEST(matrix, transpose){
  Matrix4 m;
  for(int i=0;i<4;++i) for(int j=0;j<4;++j) m[i][j] = random_uniform_01();
  auto mtrans = Matrix4::Transpose(m);
  EXPECT_NEAR(m.Det(), mtrans.Det(), EPS);
}


TEST(matrix, inverse){
  Matrix4 m;
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j) m[i][j] = random_double(-10,10);
  Matrix4 minv = Matrix4::Inverse(m);

  EXPECT_EQ(m*minv, Matrix4::I());
  EXPECT_EQ(minv*m, Matrix4::I());

  for(int i=0;i<4;++i){ m[1][i]=i; m[2][i]=5*i; }
  EXPECT_THROW(Matrix4::Inverse(m), std::runtime_error);
}

TEST(matrix, identity){
  Matrix4 a,b;
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j)
      a[i][j] = random_double(-5,5), b[i][j] = random_double(-5,5);
  auto abinv = Matrix4::Inverse(a*b);

  EXPECT_EQ(Matrix4::Inverse(a), b*abinv);
}
