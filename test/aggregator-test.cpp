#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <aggregator.hpp>
#include <bsdf.hpp>
#include <util/util.hpp>

#include <algorithm>
#include <memory>
#include <numeric>

class FakeShape : public IShape {
 public:
  FakeShape(AABB box, double time) : bbox_(std::move(box)), time_(time) {}

  HitRecord Hit(const Ray& r, const Interval<double>& interval) const override {
    if (time_ > 0.0 && bbox_.isHitIn(r, interval))
      return HitRecord::Create(time_, r.At(time_), Point2(0, 0),
                               Normal{0, 1, 0});
    else
      return HitRecord();
  }

  AABB GetBbox() const override { return bbox_; }

 private:
  AABB bbox_;
  double time_;
};

class AggregatorTest : public ::testing::Test {
 private:
  AABB rand_bbox(void) const {
    return AABB{(Point3)Vector3::Random(-100, 100),
                (Point3)Vector3::Random(-100, 100)};
  }

 protected:
  void Init(const int hittable_count = 0, const int unhittable_count = 0) {
    hittables.reserve(hittable_count);
    unhittables.reserve(unhittable_count);
    for (int i = 0; i < hittable_count; ++i) {
      auto shape =
          std::make_shared<FakeShape>(rand_bbox(), random_double(1, 100));
      auto obj = std::make_shared<Primitive>(shape, /*material=*/nullptr,
                                             /*transform=*/nullptr);
      hittables.push_back(obj);
    }
    for (int i = 0; i < unhittable_count; ++i) {
      auto shape = std::make_shared<FakeShape>(rand_bbox(), -1);
      auto obj = std::make_shared<Primitive>(shape, /*material=*/nullptr,
                                             /*transform=*/nullptr);
      unhittables.push_back(obj);
    }

    std::vector<std::shared_ptr<Primitive>> primitives;
    primitives.insert(primitives.end(), hittables.begin(), hittables.end());
    primitives.insert(primitives.end(), unhittables.begin(), unhittables.end());

    aggregator = std::make_unique<BVT>(primitives);
  }

  std::unique_ptr<BVT> aggregator;
  std::vector<std::shared_ptr<Primitive>> hittables;
  std::vector<std::shared_ptr<Primitive>> unhittables;
};

TEST_F(AggregatorTest, bbox) {
  Init(10000, 10000);
  auto merge_bbox = [](AABB lhs, const std::shared_ptr<Primitive>& rhs) {
    return AABB{lhs, rhs->GetBbox()};
  };
  auto hittables_box =
      std::accumulate(hittables.cbegin(), hittables.cend(), AABB(), merge_bbox);
  auto unhittables_box = std::accumulate(
      unhittables.cbegin(), unhittables.cend(), AABB(), merge_bbox);
  auto box = AABB{hittables_box, unhittables_box};

  EXPECT_EQ(box, aggregator->GetBbox());
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
                           [&rec](const std::shared_ptr<Primitive>& ptr) {
                             return ptr.get() == rec.primitive;
                           });

    if (!rec.hits)
      EXPECT_EQ(it, hittables.cend());
    else {
      ASSERT_NE(it, hittables.cend());
      EXPECT_EQ((*it).get(), rec.primitive);
    }
  }
}

TEST_F(AggregatorTest, SingleObject) {
  auto obj = std::make_shared<Primitive>(
      std::make_shared<FakeShape>(AABB{Point3(0, 0, 0), Point3(1, 1, 1)}, 1.0),
      nullptr, nullptr);
  std::vector<std::shared_ptr<Primitive>> primitives{obj};
  aggregator = std::make_unique<BVT>(primitives);

  Ray r(Point3(0.5, 0.5, -1), Vector3(0, 0, 1));
  auto rec = aggregator->Hit(r, Interval<double>::Positive());

  ASSERT_TRUE(rec.hits);
  EXPECT_EQ(rec.primitive, obj.get());
  EXPECT_TRUE(aggregator->GetBbox().Contains(obj->GetBbox()));
}
