#include<gtest/gtest.h>

#include<util/util.hpp>

#include<cmath>
#include<format>


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
