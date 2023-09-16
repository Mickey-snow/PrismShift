#include<gtest/gtest.h>
#include "constant.hpp"

#include<util/util.hpp>

#include<cmath>
#include<format>

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

class TransformationTest : public ::testing::Test{
protected:
  void SetUp() override{
    pt = Point3{random_double(-5,5), random_double(-5,5), random_double(-5,5)};
    vec = Vector3{random_double(-5,5), random_double(-5,5), random_double(-5,5)};
    norm = Normal{random_double(-5,5), random_double(-5,5), random_double(-5,5)};
  }

  Point3 pt;
  Vector3 vec;
  Normal norm;
};

TEST_F(TransformationTest, translate){
  double dx=random_double(-10,10), dy=random_double(-10,10), dz=random_double(-10,10);
  auto translate = Transformation::Translate(dx,dy,dz);

  auto translatedpt = translate(pt);
  auto movedpt = pt + Vector3{dx,dy,dz};
  EXPECT_EQ(translatedpt, movedpt) << "translation matrix failed for point v=" <<pt<<std::format("and dx={} dy={} dz={}", dx,dy,dz);

  auto translatedvec = translate(vec);
  EXPECT_EQ(translatedvec, vec) << "translation matrix failed for vector v="<<vec<<std::format("and dx={} dy={} dz={}", dx,dy,dz);

  auto translatedn = translate(norm);
  EXPECT_EQ(translatedn, norm.Normalize()) << "translation matrix failed for normal v="<<norm<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  
}

TEST_F(TransformationTest, scale){
  auto normal = Normal{1,1,0}.Normalize();
  auto scale = Transformation::Scale(Vector3{2,1,1});
  auto expect_normal = Normal{1,2,0}.Normalize();

  EXPECT_EQ(scale(normal), expect_normal);
}

TEST(Transformation, rotateXyzAxisn){
  auto transform = Transformation::RotateY(-4.636991) *
    Transformation::RotateZ(2.437875) *
    Transformation::RotateX(-1.50796);
  Vector3 v{5,1,7};

  Vector3 vp = transform(v);
  EXPECT_NEAR(vp.x(), -0.81511, 0.05);
  EXPECT_NEAR(vp.y(), 2.01684, 0.05);
  EXPECT_NEAR(vp.z(), -8.3826, 0.05);
}

TEST(Transformation, rotateFrto){
  Vector3 fr{random_uniform_01(), random_uniform_01(), random_uniform_01()},
    to{random_uniform_01(), random_uniform_01(), random_uniform_01()};
  fr = fr.Normalize(), to = to.Normalize();
  auto frtoRotate = Transformation::RotateFrTo(fr,to);

  EXPECT_EQ(frtoRotate(fr), to);
}

TEST(Transformation, chainedTransformation){
  auto trans = Transformation::Translate(10,1.5,1.5) *
    Transformation::Scale(Vector3{1.5,1.5,1.5});

  Point3 P(2.0/3,2.0/3,1.0/3), P2(11,2.5,2);
  EXPECT_EQ(trans(P), P2);
}
