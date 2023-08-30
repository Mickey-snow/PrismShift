#include<random>
#include<ctime>

#include "random.hpp"

double random_uniform_01(void){
  static std::uniform_real_distribution<double> distribution(0.0,1.0);
  static std::mt19937 generator(time(0));
  return distribution(generator);
}
