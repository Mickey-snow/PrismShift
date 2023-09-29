#include<gtest/gtest.h>
#include "constant.hpp"

#include<util/util.hpp>

#include<cmath>
#include<format>


class MatrixTest : public ::testing::Test{
protected:
  void SetUp() override{
    I = Matrix4::I();

    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	mat[i][j] = i+j*i+4;

    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	randMat[i][j] = random_uniform_01();

    det0mat = det20mat;
    for(int i=0;i<4;++i){ det0mat[1][i]=i; det0mat[2][i]=5*i; }
  }

  Matrix4 I;
  Matrix4 emptymat;
  Matrix4 mat;
  Matrix4 randMat;
  Matrix4 det20mat{5,-7,2,2,0,3,0,-4,-5,-8,0,3,0,5,0,-6};
  Matrix4 det0mat;
  
  const double EPS = 1e-5;
};

TEST_F(MatrixTest, unchangedAfterTimesIdentityMat){
  for(const auto& m : {I,emptymat,mat,randMat,det20mat}){
    EXPECT_EQ(m, m*I);
    EXPECT_EQ(m, I*m);
  }
}

TEST_F(MatrixTest, determinant){
  EXPECT_NEAR(det20mat.Det(),20,EPS);
  EXPECT_NEAR(I.Det(), 1, EPS);
}

TEST_F(MatrixTest, detUnchangeAfterTranspose){
  for(const auto& m : {I,emptymat,mat,randMat,det20mat}){
    auto mtrans = Matrix4::Transpose(m);
    EXPECT_NEAR(m.Det(), mtrans.Det(), EPS);
  }
}


TEST_F(MatrixTest, productWithInverseIsIdentityMat){
  for(const auto& m : {I,randMat,det20mat}){
    Matrix4 minv = Matrix4::Inverse(m);

    EXPECT_EQ(m*minv, Matrix4::I());
    EXPECT_EQ(minv*m, Matrix4::I());
  }
}

TEST_F(MatrixTest, inverseDet0MatrixThrowsError){
  for(const auto& m : {det0mat, emptymat}){
    ASSERT_DOUBLE_EQ(m.Det(), 0) << "at Matrix4::Det(): matrix "<< m <<" has determinant differ than 0";
    EXPECT_THROW(Matrix4::Inverse(m), std::runtime_error) << "at Matrix4::Inverse(): calculated the inverse of matrix "<<m<<" without throwing an error";
  }
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

TEST(Transformation, rotateNormalFrTo){
  Normal norm = (Normal)Vector3::Random_Unit();
  Vector3 in_direction = Vector3::Random(-10,10);

  auto deprecated_ray_reflect_direction = [](const Normal& n, const Vector3& ri){
    Vector3 nv = (Vector3)n;
    Vector3 scaled_normal = nv * (-Vector3::Dot(ri, nv));
    return ri + 2*scaled_normal;
  };
  Vector3 out_direction = deprecated_ray_reflect_direction(norm, in_direction);

  auto frtoRotate = Transformation::RotateFrTo((Vector3)norm, Vector3{0,0,1});
  in_direction = frtoRotate(in_direction);
  Vector3 out_direction_local = Vector3{in_direction.x(), in_direction.y(), -in_direction.z()};

  EXPECT_EQ(out_direction, frtoRotate.Inverse()(out_direction_local));
}

TEST(Transformation, chainedTransformation){
  auto trans = Transformation::Translate(10,1.5,1.5) *
    Transformation::Scale(Vector3{1.5,1.5,1.5});

  Point3 P(2.0/3,2.0/3,1.0/3), P2(11,2.5,2);
  EXPECT_EQ(trans(P), P2);
}
