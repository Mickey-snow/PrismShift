#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <aggregator.hpp>
#include <bsdf.hpp>
#include <util/util.hpp>

#include <algorithm>
#include <memory>
#include <numeric>

class fPrimitive : public IPrimitive {
 public:
  fPrimitive(const AABB& box, const double time) : m_bbox(box), m_time(time) {}

  Hit_record Hit(const Ray& r, const Interval<double>& t) const override {
    if (m_time > 0 && m_bbox.isHitIn(r, t))
      return Hit_record::ORTN(this, r, m_time, Normal{0, 1, 0});
    else
      return Hit_record{};
  }

  AABB Get_Bbox() const override { return m_bbox; }
  BSDF CalcBSDF(const Hit_record&) const override { throw; }

 private:
  AABB m_bbox;
  double m_time;
};

class AggregatorTest : public ::testing::Test {
 private:
  AABB rand_bbox(void) const {
    return AABB{(Point3)Vector3::Random(-100, 100),
                (Point3)Vector3::Random(-100, 100)};
  }

 protected:
  void Init(const int hittable_count = 0, const int unhittable_count = 0) {
    for (int i = 0; i < hittable_count; ++i) {
      auto obj =
          std::make_shared<fPrimitive>(rand_bbox(), random_double(1, 100));
      hittables.push_back(obj);
    }
    for (int i = 0; i < unhittable_count; ++i) {
      auto obj = std::make_shared<fPrimitive>(rand_bbox(), -1);
      unhittables.push_back(obj);
    }

    std::vector<std::shared_ptr<IPrimitive>> primitives;
    primitives.insert(primitives.end(), hittables.begin(), hittables.end());
    primitives.insert(primitives.end(), unhittables.begin(), unhittables.end());

    aggregator = std::make_unique<BVT>(primitives);
  }

  std::unique_ptr<BVT> aggregator;
  std::vector<std::shared_ptr<fPrimitive>> hittables;
  std::vector<std::shared_ptr<fPrimitive>> unhittables;
};

TEST_F(AggregatorTest, bbox) {
  Init(10000, 10000);
  auto merge_bbox = [](AABB lhs, const std::shared_ptr<fPrimitive>& rhs) {
    return AABB{lhs, rhs->Get_Bbox()};
  };
  auto hittables_box =
      std::accumulate(hittables.cbegin(), hittables.cend(), AABB(), merge_bbox);
  auto unhittables_box = std::accumulate(
      unhittables.cbegin(), unhittables.cend(), AABB(), merge_bbox);
  auto box = AABB{hittables_box, unhittables_box};

  EXPECT_EQ(box, aggregator->Get_Bbox());
}

TEST_F(AggregatorTest, hit) {
  Init(10000, 10000);
  const int testcases = 10000;
  auto rand_ray = []() {
    return Ray((Point3)Vector3::Random(-1000, 1000), Vector3::Random_Unit());
  };

  for (int i = 0; i < testcases; ++i) {
    auto r = rand_ray();
    auto t = Interval<double>::Positive();

    auto rec = aggregator->Hit(r, t);
    auto it = std::find_if(hittables.cbegin(), hittables.cend(),
                           [&rec](const std::shared_ptr<fPrimitive>& ptr) {
                             return ptr.get() == rec.hitted_obj;
                           });

    if (!rec.isHit())
      EXPECT_EQ(it, hittables.cend());
    else {
      ASSERT_NE(it, hittables.cend());
      EXPECT_EQ((*it).get(), rec.hitted_obj);
    }
  }
}

TEST_F(AggregatorTest, SingleObject) {
  auto obj =
      std::make_shared<fPrimitive>(AABB{Point3(0, 0, 0), Point3(1, 1, 1)}, 1.0);
  std::vector<std::shared_ptr<IPrimitive>> primitives{obj};
  aggregator = std::make_unique<BVT>(primitives);

  Ray r(Point3(0.5, 0.5, -1), Vector3(0, 0, 1));
  auto rec = aggregator->Hit(r, Interval<double>::Positive());

  ASSERT_TRUE(rec.isHit());
  EXPECT_EQ(rec.hitted_obj, obj.get());
  EXPECT_TRUE(aggregator->Get_Bbox().Contains(obj->Get_Bbox()));
}
