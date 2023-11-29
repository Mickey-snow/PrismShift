#include<gtest/gtest.h>

#include<util/util.hpp>

#include<cmath>
#include<format>


class MatrixTest : public ::testing::Test{
protected:
  void SetUp() override{
    I = Matrix4::I();

    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	randiMat[i][j] = i+j*i+4;

    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	randfMat[i][j] = random_uniform_01();

    det0mat = det20mat;
    for(int i=0;i<4;++i){ det0mat[1][i]=i; det0mat[2][i]=5*i; }

    CheckSetUp();
  }

  void CheckSetUp(){
    for(int i=0;i<4;++i)
      ASSERT_DOUBLE_EQ(I[i][i], 1);

    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	ASSERT_DOUBLE_EQ(emptymat[i][j], 0);

    ASSERT_DOUBLE_EQ(det20mat.Det(), 20);

    ASSERT_DOUBLE_EQ(det0mat.Det(), 0);
  }

  Matrix4 I;
  Matrix4 emptymat;
  Matrix4 randiMat;
  Matrix4 randfMat;
  Matrix4 det20mat{5,-7,2,2,0,3,0,-4,-5,-8,0,3,0,5,0,-6};
  Matrix4 det0mat;
  
  const double EPS = 1e-5;
};

TEST_F(MatrixTest, unchangedAfterTimesIdentityMat){
  for(const auto& m : {I,emptymat,randiMat,randfMat,det20mat}){
    EXPECT_EQ(m, m*I);
    EXPECT_EQ(m, I*m);
  }
}

TEST_F(MatrixTest, determinant){
  EXPECT_NEAR(det20mat.Det(),20,EPS);
  EXPECT_NEAR(I.Det(), 1, EPS);
}

TEST_F(MatrixTest, detUnchangeAfterTranspose){
  for(const auto& m : {I,emptymat,randiMat,randfMat,det20mat}){
    auto mtrans = Matrix4::Transpose(m);
    EXPECT_NEAR(m.Det(), mtrans.Det(), EPS);
  }
}


TEST_F(MatrixTest, productWithInverseIsIdentityMat){
  for(const auto& m : {I,randfMat,det20mat}){
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



class TransformationTest : public ::testing::Test{
protected:
  struct PointData{
    Point3 origin{0,0,0};
    Point3 i{1,0,0};
    Point3 j{0,1,0};
    Point3 k{0,0,1};
    Point3 neg{-1,-4,-2};
    Point3 mix{-19,23,0};
    Point3 f{1.5,-9.3,89.64};
    Point3 rand{random_uniform_01(), random_uniform_01(), random_uniform_01()};

    std::vector<Point3> asVector(){
      return std::vector<Point3>{origin,i,j,k,neg,mix,f,rand};
    }
  }pt;

  struct VectorData{
    Vector3 zero{0,0,0};
    Vector3 i{1,0,0};
    Vector3 j{0,1,0};
    Vector3 k{0,0,1};
    Vector3 neg{-1,-4,-2};
    Vector3 mix{-19,23,0};
    Vector3 f{1.5,-9.3,89.64};
    Vector3 rand = Vector3::Random_Unit();

    std::vector<Vector3> asVector(){
      return std::vector<Vector3>{zero,i,j,k,neg,mix,f,rand};
    }
  }vec;

  struct NormalData{
    Normal i{1,0,0};
    Normal j{0,1,0};
    Normal k{0,0,1};
    Normal nonUnit{1,1,1};
    Normal f{1.5,4.5,9.123};

    std::vector<Normal> asVector(){
      return std::vector<Normal>{i,j,k,nonUnit,f};
    }
  }norm;

  const double EPS = 1e-6;
};

TEST_F(TransformationTest, translate){
  double dx=random_double(-10,10), dy=random_double(-10,10), dz=random_double(-10,10);
  auto translate = Transformation::Translate(dx,dy,dz);

  for(const auto& p : pt.asVector()){
    auto translatedpt = translate(p);
    auto movedpt = p + Vector3{dx,dy,dz};
    EXPECT_EQ(translatedpt, movedpt) << "at translateTest: failed for point v=" <<p<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }

  for(const auto& v : vec.asVector()){
    // translations should not affect vectors
    auto translatedvec = translate(v);
    EXPECT_EQ(translatedvec, v) << "at translateTest: failed for vector v="<<v<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }

  for(const auto& n : norm.asVector()){
    // translations sould not affect normals, however they will be normalized
    auto translatedn = translate(n);
    EXPECT_EQ(translatedn, n) << "at translateTest: failed for normal v="<<n<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }
}

TEST_F(TransformationTest, zeroScalingCollapseToTheOrigin){
  auto zscale = Transformation::Scale({0,0,0});

  for(const auto& p : pt.asVector()){
    auto scaled = zscale(p);
    auto origin = Point3{0,0,0};
    EXPECT_EQ(scaled, origin);
  }

  for(const auto& v : vec.asVector()){
    auto scaled = zscale(v);
    auto zerovec = Vector3{0,0,0};
    EXPECT_EQ(scaled, zerovec);
  }

  for(const auto& n : norm.asVector()){
    // normalize a normal collapsed to zero results in nans
    auto scaled = zscale(n);
    EXPECT_TRUE(std::isnan(scaled.x()));
    EXPECT_TRUE(std::isnan(scaled.y()));
    EXPECT_TRUE(std::isnan(scaled.z()));
  }
}

TEST_F(TransformationTest, identityscalingLeaveUnchanged){
  for(const auto& iscale : {Transformation::Scale({1,1,1}), Transformation::Scale(vec.rand, 1)}){
    for(const auto& p : pt.asVector()){
      auto scaled = iscale(p);
      EXPECT_EQ(scaled, p);
    }

    for(const auto& v : vec.asVector()){
      auto scaled = iscale(v);
      EXPECT_EQ(scaled, v);
    }

    for(const auto& n : norm.asVector()){
      auto scaled = iscale(n);
      EXPECT_EQ(scaled, n);
    }
  }
}

TEST_F(TransformationTest, nonuniformScaling){
  double sx=random_uniform_01(), sy=random_uniform_01(), sz=random_uniform_01();
  auto nuscale = Transformation::Scale({sx,sy,sz});

  for(const auto& p : pt.asVector()){
    auto scaled = nuscale(p);
    auto expect = Point3{p.x()*sx, p.y()*sy, p.z()*sz};
    EXPECT_EQ(scaled, expect);
  }

  for(const auto& v : vec.asVector()){
    auto scaled = nuscale(v);
    auto expect = Vector3{v.x()*sx, v.y()*sy, v.z()*sz};
    EXPECT_EQ(scaled, expect);
  }

  for(const auto& n : norm.asVector()){
    auto perpendicular = Vector3::Cross(Vector3{2,3,4}, n);
    auto scaled = nuscale(n);
    perpendicular = nuscale(perpendicular);
    EXPECT_NEAR(Vector3::Dot(perpendicular, (Vector3)scaled), 0, EPS);
  }
}

TEST_F(TransformationTest, rotateXyzAxis){
  auto transform = Transformation::RotateY(-4.636991) *
    Transformation::RotateZ(2.437875) *
    Transformation::RotateX(-1.50796);
  Vector3 v{5,1,7};

  Vector3 vp = transform(v);
  EXPECT_NEAR(vp.x(), -0.81511, 0.05);
  EXPECT_NEAR(vp.y(), 2.01684, 0.05);
  EXPECT_NEAR(vp.z(), -8.3826, 0.05);
}

TEST_F(TransformationTest, rotateFrto){
  for(auto v : vec.asVector()){
    if(v.Near_Zero()) continue;

    v = v.Normalized();
    auto perpendicular = Vector3::Cross(v+Vector3{1,2,3}, v);
    auto rotateTo = Vector3::Random_Unit();

    auto ftRotate = Transformation::RotateFrTo(v, rotateTo);
    auto vrotated = ftRotate(v);

    EXPECT_EQ(vrotated, rotateTo) << "v=" << v << "  rotateTo=" << rotateTo;
    EXPECT_TRUE(Vector3::isPerpendicular(vrotated, ftRotate(perpendicular)));
  }
}

TEST_F(TransformationTest, rotateNormalFrTo){
  Normal norm = (Normal)Vector3::Random_Unit();
  Vector3 in_direction = Vector3::Random(-10,10);

  auto deprecated_ray_reflect_direction = [](const Normal& n, const Vector3& ri){
    Vector3 nv = (Vector3)n;
    Vector3 scaled_normal = nv * (-Vector3::Dot(ri, nv));
    return ri + 2*scaled_normal;
  };
  Vector3 out_direction = deprecated_ray_reflect_direction(norm, in_direction);

  auto frtoRotate = Transformation::RotateFrTo((Vector3)norm, Vector3{0,0,1});
  ASSERT_EQ(frtoRotate(norm), Normal(0,0,1));
  in_direction = frtoRotate(in_direction);
  Vector3 out_direction_local = Vector3{in_direction.x(), in_direction.y(), -in_direction.z()};

  EXPECT_EQ(out_direction, frtoRotate.Inverse()(out_direction_local));
}

TEST_F(TransformationTest, chainedTransformation){
  auto t1 = Transformation::Translate(Vector3::Random(-10,10));
  auto t2 = Transformation::Scale(Vector3::Random(-5,5));
  auto t3 = Transformation::Rotate(Vector3::Random_Unit(),random_uniform_01());
  auto chained = t1*t2*t3;

  for(auto p : pt.asVector())
    EXPECT_EQ(chained(p), p.Transform(t3).Transform(t2).Transform(t1));
  for(auto v : vec.asVector())
    EXPECT_EQ(chained(v), v.Transform(t3).Transform(t2).Transform(t1));
  for(auto n : norm.asVector())
    EXPECT_EQ(chained(n), n.Transform(t3).Transform(t2).Transform(t1));
}
