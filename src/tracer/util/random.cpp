#include <random>

#include "random.hpp"

static std::mt19937 gen([] {
  std::random_device rd;
  return rd();
}());

Float random_uniform_01() {
  static std::uniform_real_distribution<Float> distribution(0.0, 1.0);
  return distribution(gen);
}

Float random_double(Float min, Float max) {
  std::uniform_real_distribution<Float> distribution(min, max);
  return distribution(gen);
}

int random_int(int min, int max) {
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(gen);
}
