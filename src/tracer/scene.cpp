#include "scene.hpp"

#include <aggregator.hpp>
#include <primitive.hpp>
#include <util/util.hpp>

Scene::Scene(std::vector<std::shared_ptr<Primitive>> objs)
    : objs_(std::move(objs)), aggregator_(std::make_unique<BVT>(objs_)) {}

HitRecord Scene::Hit(Ray r, Interval<double> time) const {
  return aggregator_->Hit(r, time);
}

AABB Scene::GetBbox(void) const { return aggregator_->GetBbox(); }

void Scene::SetBackground(std::function<Color(Ray)> fn) {
  background_fn_ = std::move(fn);
}

Color Scene::Background(Ray ray) const {
  return std::invoke(background_fn_, std::move(ray));
}
