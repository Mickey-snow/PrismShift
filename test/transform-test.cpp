#include<gtest/gtest.h>

#include<util/util.hpp>

#include<cmath>
#include<format>
#include<memory>


class TransformTest : public ::testing::Test{};

template<class translate_t>
class TranslateTest : public TransformTest{
public:

  void SetUp() override{
    tr1 = std::make_unique<translate_t>(translate_t::Translate(3,2,1));
    tr2 = std::make_unique<translate_t>(translate_t::Translate(1,1,1));
    tr3 = std::make_unique<translate_t>(translate_t::Translate(-1.5,2,-89));
  }
  
  std::unique_ptr<ITransformation> tr1,tr2,tr3;  
};

using translation_impl = ::testing::Types<MatrixTransformation, VectorTranslate>;
TYPED_TEST_SUITE(TranslateTest, translation_impl);

TYPED_TEST(TranslateTest, Points){
  Point3 origin{0,0,0};
  Point3 i{1,0,0};
  Point3 j{0,1,0};
  Point3 k{0,0,1};
  Point3 neg{-1,-4,-2};
  Point3 mix{-19,23,0};
  Point3 f{1.5,-9.3,89.64};
  Point3 rand{random_uniform_01(), random_uniform_01(), random_uniform_01()};
  std::vector<Point3> points{origin,i,j,k,neg,mix,f,rand};

  for(const auto& p : points){
    Point3 expected1{p.x()+3, p.y()+2, p.z()+1};
    EXPECT_EQ(this->tr1->Doit(p), expected1);
    EXPECT_EQ(this->tr1->Undo(expected1), p);

    Point3 expected2{p.x()+1, p.y()+1, p.z()+1};
    EXPECT_EQ(this->tr2->Doit(p), expected2);
    EXPECT_EQ(this->tr2->Undo(expected2), p);

    Point3 expected3{p.x()-1.5, p.y()+2, p.z()-89};
    EXPECT_EQ(this->tr3->Doit(p), expected3);
    EXPECT_EQ(this->tr3->Undo(expected3), p);
  }
}

TYPED_TEST(TranslateTest, Vectors){
  // translate should not affect vectors
  Vector3 v1{1,2,3},zero{0,0,0},v2{1000,2000,3000};
  EXPECT_EQ(v1,this->tr1->Doit(v1));
  EXPECT_EQ(zero,this->tr1->Doit(zero));
  EXPECT_EQ(v2,this->tr1->Doit(v2));
  EXPECT_EQ(v1,this->tr1->Undo(v1));
  EXPECT_EQ(zero,this->tr1->Undo(zero));
  EXPECT_EQ(v2,this->tr1->Undo(v2));

}

TYPED_TEST(TranslateTest, Normals){
  // translate should not affect normals
  Normal n{-1.0/sqrt(2.0),0,1.0/sqrt(2.0)};
  EXPECT_EQ(n, this->tr1->Doit(n));
  EXPECT_EQ(n, this->tr2->Doit(n));
  EXPECT_EQ(n, this->tr3->Doit(n));
  EXPECT_EQ(n, this->tr1->Undo(n));
  EXPECT_EQ(n, this->tr2->Undo(n));
  EXPECT_EQ(n, this->tr3->Undo(n));
}


template<typename scale_t>
class ScalingTest : public TransformTest{
public:
  void SetUp() override{
    tr1 = std::make_unique<scale_t>(scale_t::Scale(2,3,4));
    tr2 = std::make_unique<scale_t>(scale_t::Scale(1,-2,1.5));
    tr3 = std::make_unique<scale_t>(scale_t::Scale(0,0,0));
  }

  std::unique_ptr<ITransformation> tr1,tr2,tr3;
};

using scale_impl = ::testing::Types<MatrixTransformation, VectorScale>;
TYPED_TEST_SUITE(ScalingTest, scale_impl);

TYPED_TEST(ScalingTest, Points){
  const Point3 p1{1,2,3}, origin{0,0,0};
  EXPECT_EQ(this->tr1->Doit(p1), Point3(2,6,12));
  EXPECT_EQ(this->tr1->Undo(p1), Point3(0.5,2.0/3,0.75));
  EXPECT_EQ(this->tr1->Doit(origin), origin);
  EXPECT_EQ(this->tr1->Undo(origin), origin);

  EXPECT_EQ(this->tr2->Doit(p1), Point3(1,-4,4.5));
  EXPECT_EQ(this->tr2->Undo(p1), Point3(1,-1,2));
  EXPECT_EQ(this->tr2->Doit(origin), origin);
  EXPECT_EQ(this->tr2->Undo(origin), origin);

  EXPECT_EQ(this->tr3->Doit(p1), origin);
  EXPECT_EQ(this->tr3->Doit(origin), origin);
}

TYPED_TEST(ScalingTest, Vectors){
  const Vector3 v1{-1,-2,3}, v2{1,1,1}, zero{0,0,0};
  EXPECT_EQ(this->tr1->Doit(v1), Vector3(-2,-6,12));
  EXPECT_EQ(this->tr1->Undo(v1), Vector3(-0.5,-2.0/3,0.75));
  EXPECT_EQ(this->tr1->Doit(v2), Vector3(2,3,4));
  EXPECT_EQ(this->tr1->Undo(v2), Vector3(1.0/2,1.0/3,1.0/4));

  EXPECT_EQ(this->tr3->Doit(v1), zero);
  EXPECT_EQ(this->tr3->Doit(v2), zero);
}

TYPED_TEST(ScalingTest, Normals){
  const Normal n{1.0/sqrt(2.0), 1.0/sqrt(2.0), 0};
  const Vector3 t{1,-1,3.2};
  
  Normal np = this->tr1->Doit(n);
  Vector3 tp = this->tr1->Doit(t);
  EXPECT_DOUBLE_EQ(np.Dot(tp), 0.0);
  EXPECT_EQ(this->tr1->Undo(np), n);

  np = this->tr2->Doit(n);
  tp = this->tr2->Doit(t);
  EXPECT_DOUBLE_EQ(np.Dot(tp), 0.0);
  EXPECT_EQ(this->tr2->Undo(np), n);
}


template<class rotate_t>
class RotateTest : public TransformTest{
public:
  decltype(auto) axisTr(basic_vector<double,3> n, double angle){
    return std::make_shared<rotate_t>(std::move(rotate_t::Rotate(n,angle)));
  }

  decltype(auto) frtoTr(Vector3 fr, Vector3 to){
    return std::make_shared<rotate_t>(std::move(rotate_t::RotateFrTo(fr,to)));
  }

  decltype(auto) alignxyzTr(const basic_vector<Vector3, 3>& basis){
    return std::make_shared<rotate_t>(std::move(rotate_t::AlignXYZ(basis)));
  }
};

using rotate_impl = ::testing::Types<MatrixTransformation, QuaternionTransform>;
TYPED_TEST_SUITE(RotateTest, rotate_impl);

TYPED_TEST(RotateTest, simpleAxis){
  Vector3 v{1,0,0};
  Normal n{0,1,0};
  double angle = pi/2;
  auto tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), Vector3(0,0,1));

  v = Vector3{0,0,1};
  n = Normal{0,1,0};
  angle = -45.0 / 180 * pi;
  tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), Vector3(sqrt(2.0)/2,0,sqrt(2.0)/2));
}

TYPED_TEST(RotateTest, diagonalAxis){
  Vector3 v{0,1,0};
  Normal n{1,1,1};
  double angle = 120.0 /180*pi;
  auto tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), Vector3(1,0,0));
}

TYPED_TEST(RotateTest, smallAngleAxis){
  Vector3 v{1,1,1};
  Normal n{1,0,0};
  double angle = 10.0 / 180*pi;
  auto tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), Vector3(1,1.15845593,0.81115958));
}

TYPED_TEST(RotateTest, zeroVector){
  Vector3 v{0,0,0};
  auto randNormal = [](){ return Normal{random_uniform_01(), random_uniform_01(), random_uniform_01()}; };
  auto n=randNormal(), np=randNormal();
  double angle = pi*2 * random_uniform_01();
  auto tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), v);
  
  tr = this->frtoTr((Vector3)n,(Vector3)np);
  EXPECT_EQ(tr->Doit(v), v);
}

TYPED_TEST(RotateTest, rotate180DegAxis){
  Vector3 v{random_uniform_01(), 0,0};
  Normal n{0,1,0};
  double angle = pi;
  auto tr = this->axisTr(n,angle);
  EXPECT_EQ(tr->Doit(v), -v);
}

TYPED_TEST(RotateTest, basicfrto){
  Vector3 fr,to;
  fr = Vector3(0,0,1), to = Vector3(0,0,1); // same direction
  auto tr = this->frtoTr(fr,to);
  EXPECT_EQ(tr->Doit(fr), to); EXPECT_EQ(tr->Undo(to), fr);

  to = Vector3(0,1,0);
  tr = this->frtoTr(fr,to);
  EXPECT_EQ(tr->Doit(fr), to); EXPECT_EQ(tr->Undo(to), fr);

  to = Vector3(1,0,0);
  tr = this->frtoTr(fr,to);
  EXPECT_EQ(tr->Doit(fr), to); EXPECT_EQ(tr->Undo(to), fr);
}

TYPED_TEST(RotateTest, randomFrto){
  static constexpr auto testcases = 100;
  for(int i=0;i<testcases;++i){
    auto randUnitVec = [](){ return Vector3(random_double(-1000,1000), random_double(-1000,1000), random_double(-1000,1000)).Normalized(); };
    auto fr=randUnitVec(), to=randUnitVec();
    auto tr = this->frtoTr(fr,to);
    EXPECT_EQ(tr->Doit(fr), to); EXPECT_EQ(tr->Undo(to), fr);
  }
}

TYPED_TEST(RotateTest, basicAlign){
  basic_vector<Vector3, 3> basis{Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1)};
  Vector3 world{6,7,8}, local;
  auto tr = this->alignxyzTr(basis); local = world;
  EXPECT_EQ(tr->Doit(world), local); EXPECT_EQ(tr->Undo(local), world);

  basis = basic_vector<Vector3,3>{Vector3(0,-1,0), Vector3(1,0,0), Vector3(0,0,1)};
  tr = this->alignxyzTr(basis); local = Vector3(-7,6,8);
  EXPECT_EQ(tr->Doit(world), local); EXPECT_EQ(tr->Undo(local), world);

  basis = basic_vector<Vector3,3>{Vector3(0,-1,0), Vector3(0,0,-1), Vector3(1,0,0)};
  tr = this->alignxyzTr(basis); local = Vector3(-7,-8,6);
  EXPECT_EQ(tr->Doit(world), local); EXPECT_EQ(tr->Undo(local), world);
}

TYPED_TEST(RotateTest, randomAlign){
  static constexpr auto testcases = 100;
  for(int _=0;_<testcases;++_){
    auto randVec = [](){ return Vector3(random_double(-1000,1000), random_double(-1000,1000), random_double(-1000,1000)); };
    auto i=randVec().Normalized(), j=randVec(); auto k=i.Cross(j).Normalized(); j=k.Cross(i);
    basic_vector<Vector3, 3> basis{i,j,k};
    Vector3 world = randVec();
    auto tr = this->alignxyzTr(basis);
    Vector3 local = tr->Doit(world);
    auto expr = local.x()*i + local.y()*j + local.z()*k;
    EXPECT_EQ(expr, world); EXPECT_EQ(tr->Undo(local), world);
  }
}


class MatTransformationTest : public ::testing::Test{
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

TEST_F(MatTransformationTest, translate){
  double dx=random_double(-10,10), dy=random_double(-10,10), dz=random_double(-10,10);
  auto translate = MatrixTransformation::Translate(dx,dy,dz);

  for(const auto& p : pt.asVector()){
    auto translatedpt = translate.Doit(p);
    auto movedpt = p + Vector3{dx,dy,dz};
    EXPECT_EQ(translatedpt, movedpt) << "at translateTest: failed for point v=" <<p<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }

  for(const auto& v : vec.asVector()){
    // translations should not affect vectors
    auto translatedvec = translate.Doit(v);
    EXPECT_EQ(translatedvec, v) << "at translateTest: failed for vector v="<<v<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }

  for(const auto& n : norm.asVector()){
    // translations sould not affect normals, however they will be normalized
    auto translatedn = translate.Doit(n);
    EXPECT_EQ(translatedn, n) << "at translateTest: failed for normal v="<<n<<std::format("and dx={} dy={} dz={}", dx,dy,dz);
  }
}

TEST_F(MatTransformationTest, zeroScalingCollapseToTheOrigin){
  auto zscale = MatrixTransformation::Scale({0,0,0});

  for(const auto& p : pt.asVector()){
    auto scaled = zscale.Doit(p);
    auto origin = Point3{0,0,0};
    EXPECT_EQ(scaled, origin);
  }

  for(const auto& v : vec.asVector()){
    auto scaled = zscale.Doit(v);
    auto zerovec = Vector3{0,0,0};
    EXPECT_EQ(scaled, zerovec);
  }

  for(const auto& n : norm.asVector()){
    // normalize a normal collapsed to zero results in nans
    auto scaled = zscale.Doit(n);
    EXPECT_TRUE(std::isnan(scaled.x()));
    EXPECT_TRUE(std::isnan(scaled.y()));
    EXPECT_TRUE(std::isnan(scaled.z()));
  }
}

TEST_F(MatTransformationTest, identityscalingLeaveUnchanged){
  for(const auto& iscale : {MatrixTransformation::Scale({1,1,1}), MatrixTransformation::Scale(vec.rand, 1)}){
    for(const auto& p : pt.asVector()){
      auto scaled = iscale.Doit(p);
      EXPECT_EQ(scaled, p);
    }

    for(const auto& v : vec.asVector()){
      auto scaled = iscale.Doit(v);
      EXPECT_EQ(scaled, v);
    }

    for(const auto& n : norm.asVector()){
      auto scaled = iscale.Doit(n);
      EXPECT_EQ(scaled, n);
    }
  }
}

TEST_F(MatTransformationTest, nonuniformScaling){
  double sx=random_uniform_01(), sy=random_uniform_01(), sz=random_uniform_01();
  auto nuscale = MatrixTransformation::Scale({sx,sy,sz});

  for(const auto& p : pt.asVector()){
    auto scaled = nuscale.Doit(p);
    auto expect = Point3{p.x()*sx, p.y()*sy, p.z()*sz};
    EXPECT_EQ(scaled, expect);
  }

  for(const auto& v : vec.asVector()){
    auto scaled = nuscale.Doit(v);
    auto expect = Vector3{v.x()*sx, v.y()*sy, v.z()*sz};
    EXPECT_EQ(scaled, expect);
  }

  for(const auto& n : norm.asVector()){
    auto perpendicular = Vector3::Cross(Vector3{2,3,4}, n);
    auto scaled = nuscale.Doit(n);
    perpendicular = nuscale.Doit(perpendicular);
    EXPECT_NEAR(Vector3::Dot(perpendicular, (Vector3)scaled), 0, EPS);
  }
}

TEST_F(MatTransformationTest, rotateXyzAxis){
  auto transform = MatrixTransformation::RotateY(-4.636991) *
    MatrixTransformation::RotateZ(2.437875) *
    MatrixTransformation::RotateX(-1.50796);
  Vector3 v{5,1,7};

  Vector3 vp = transform.Doit(v);
  EXPECT_NEAR(vp.x(), -0.81511, 0.05);
  EXPECT_NEAR(vp.y(), 2.01684, 0.05);
  EXPECT_NEAR(vp.z(), -8.3826, 0.05);
}

TEST_F(MatTransformationTest, rotateFrto){
  for(auto v : vec.asVector()){
    if(v.Near_Zero()) continue;

    v = v.Normalized();
    auto perpendicular = Vector3::Cross(v+Vector3{1,2,3}, v);
    auto rotateTo = Vector3::Random_Unit();

    auto ftRotate = MatrixTransformation::RotateFrTo(v, rotateTo);
    auto vrotated = ftRotate.Doit(v);

    EXPECT_EQ(vrotated, rotateTo) << "v=" << v << "  rotateTo=" << rotateTo;
    EXPECT_TRUE(Vector3::isPerpendicular(vrotated, ftRotate.Doit(perpendicular)));
  }
}

TEST_F(MatTransformationTest, rotateNormalFrTo){
  Normal norm = (Normal)Vector3::Random_Unit();
  Vector3 in_direction = Vector3::Random(-10,10);

  auto deprecated_ray_reflect_direction = [](const Normal& n, const Vector3& ri){
    Vector3 nv = (Vector3)n;
    Vector3 scaled_normal = nv * (-Vector3::Dot(ri, nv));
    return ri + 2*scaled_normal;
  };
  Vector3 out_direction = deprecated_ray_reflect_direction(norm, in_direction);

  auto frtoRotate = MatrixTransformation::RotateFrTo((Vector3)norm, Vector3{0,0,1});
  ASSERT_EQ(frtoRotate.Doit(norm), Normal(0,0,1));
  in_direction = frtoRotate.Doit(in_direction);
  Vector3 out_direction_local = Vector3{in_direction.x(), in_direction.y(), -in_direction.z()};

  EXPECT_EQ(out_direction, frtoRotate.Undo(out_direction_local));
}

TEST_F(MatTransformationTest, chainedTransformation){
  auto t1 = MatrixTransformation::Translate(Vector3::Random(-10,10));
  auto t2 = MatrixTransformation::Scale(Vector3::Random(-5,5));
  auto t3 = MatrixTransformation::Rotate(Vector3::Random_Unit(),random_uniform_01());
  auto chained = t1*t2*t3;

  for(auto p : pt.asVector())
    EXPECT_EQ(chained.Doit(p), p.Transform(t3).Transform(t2).Transform(t1));
  for(auto v : vec.asVector())
    EXPECT_EQ(chained.Doit(v), v.Transform(t3).Transform(t2).Transform(t1));
  for(auto n : norm.asVector())
    EXPECT_EQ(chained.Doit(n), n.Transform(t3).Transform(t2).Transform(t1));
}
