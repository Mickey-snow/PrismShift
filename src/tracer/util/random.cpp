#include<random>

#include "random.hpp"

static std::mt19937 gen([]{
  std::random_device rd;
  return rd();
 }());

double random_uniform_01(void){
  static std::uniform_real_distribution<double> distribution(0.0,1.0);
  return distribution(gen);
}

double random_double(const double& min, const double& max){
  std::uniform_real_distribution<double> distribution(min,max);
  return distribution(gen);
}

int random_int(const int& min, const int& max){
  std::uniform_int_distribution<int> distribution(min,max);
  return distribution(gen);
}
