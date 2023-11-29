#include<gtest/gtest.h>
#include<gmock/gmock.h>

#include<aggregator.hpp>
#include<bsdf.hpp>
#include<util/util.hpp>

#include<memory>
#include<algorithm>
#include<numeric>

class fPrimitive : public IPrimitive{
public:
  fPrimitive(const AABB& box, const double time) : m_bbox(box), m_time(time) {}

  Hit_record Hit(const Ray& r, const Interval<double>& t) const override{
    if(m_time>0 && m_bbox.isHitIn(r, t))
      return Hit_record::ORTN(this, r, m_time, Normal{0,1,0});
    else return Hit_record{};
  }

  AABB Get_Bbox() const override{ return m_bbox; }
  BSDF CalcBSDF(const Hit_record&) const override{ throw; }
  
private:
  AABB m_bbox;
  double m_time;
};

class AggregatorTest : public ::testing::Test{
private:
  const int hittable_count = 10000;
  const int unhittable_count = 10000;

  AABB rand_bbox(void) const {
    return AABB{(Point3)Vector3::Random(-100,100), (Point3)Vector3::Random(-100,100)};
  }
  
protected:
  void SetUp() override{
    for(int i=0;i<hittable_count;++i){
      auto obj = new fPrimitive(rand_bbox(), random_double(1,100));
      hittables.push_back(obj);
    }
    for(int i=0;i<unhittable_count;++i){
      auto obj = new fPrimitive(rand_bbox(), -1);
      unhittables.push_back(obj);
    }

    std::vector<std::shared_ptr<IPrimitive>> primitives;
    std::transform(hittables.cbegin(), hittables.cend(), std::back_inserter(primitives),
		   [](const fPrimitive* ptr){ return std::make_shared<fPrimitive>(*ptr); });
    std::transform(unhittables.cbegin(), unhittables.cend(), std::back_inserter(primitives),
		   [](const fPrimitive* ptr){ return std::make_shared<fPrimitive>(*ptr); });
    aggregator = new BVT(primitives);
  }

  void TearDown() override{
    delete aggregator;
    for(auto& it : hittables) delete it;
    for(auto& it : unhittables) delete it;
  }

  BVT* aggregator;

  std::vector<fPrimitive*> hittables;
  std::vector<fPrimitive*> unhittables;
};


TEST_F(AggregatorTest, bbox){
  auto merge_bbox = [](AABB lhs, IPrimitive* rhs){
    return AABB{lhs, rhs->Get_Bbox()}; };
  auto hittables_box = std::accumulate(hittables.cbegin(), hittables.cend(), AABB(), merge_bbox);
  auto unhittables_box = std::accumulate(unhittables.cbegin(), unhittables.cend(), AABB(), merge_bbox);
  auto box = AABB{hittables_box, unhittables_box};

  EXPECT_EQ(box, aggregator->Get_Bbox());
}

// fixme
// TEST_F(AggregatorTest, hit){
//   const int testcases = 10000;
//   auto rand_ray = [](){ return Ray((Point3)Vector3::Random(-1000,1000), Vector3::Random_Unit()); };

//   for(int i=0;i<testcases;++i){
//     auto r = rand_ray();
//     auto t = Interval<double>::Positive();

//     auto rec = aggregator->Hit(r,t);
//     auto it = std::find(hittables.cbegin(), hittables.cend(), rec.hitted_obj);

//     if(!rec.isHit()) EXPECT_EQ(it, hittables.cend());
//     else{
//       ASSERT_NE(it, hittables.cend());
//       EXPECT_EQ(*it, rec.hitted_obj);
//     }
//   }
// }
