#include<random>
#include<ctime>

#include "random.hpp"

double random_uniform_01(void){
  static std::uniform_real_distribution<double> distribution(0.0,1.0);
  static std::mt19937 generator(time(0));
  return distribution(generator);
}

double random_double(const double& min, const double& max){
  return min + (max-min)*random_uniform_01();
}

int random_int(const int& min, const int& max){
  return static_cast<int>(random_double(min, max+0.999));
}
