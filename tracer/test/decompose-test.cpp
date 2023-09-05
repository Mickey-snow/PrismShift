#include<gtest/gtest.h>
#include "vec-comparer.hpp"
#include "constant.hpp"

#include<cmath>

#include<util/decomposer.hpp>
#include<util/vector3.hpp>
#include<util/random.hpp>

TEST(decomposer, generalDecompose){
  Vector3 i=Vector3::Random_Unit(), j=Vector3::Random_Unit(), k=Vector3::Random_Unit();
  double a = random_uniform_01(), b = random_uniform_01(), c = random_uniform_01();
  Vector3 p = a*i + b*j + c*k;
  
  Decomposer3d dec(i,j,k);
  auto alpha = dec.Componenti(p),
    beta = dec.Componentj(p),
    gamma = dec.Componentk(p);

  EXPECT_TRUE(Vec3eq(Vector3(a,b,c), Vector3(alpha,beta,gamma)));
}

TEST(decomposer, planeDecompose){
  Vector3 i=Vector3::Random_Unit(), j=Vector3::Random_Unit();
  Vector3 k = Vector3::Cross(i,j);
  double a = random_uniform_01(), b = random_uniform_01(), c = random_uniform_01();
  Vector3 p = a*i + b*j;

  Decomposer3d dec(i,j);
  auto alpha = dec.Componenti(p),
    beta = dec.Componentj(p),
    gamma = dec.Componentk(p);
  EXPECT_TRUE(Vec3eq(Vector3(alpha,beta,gamma), a,b,0));

  a = random_uniform_01(); b = random_uniform_01();
  p = a*i+b*j+c*k;
  alpha = dec.Componenti(p); beta = dec.Componentj(p); gamma = dec.Componentk(p);
  EXPECT_TRUE(Vec3eq(Vector3(alpha,beta,gamma), Vector3(a,b,c)));
}

TEST(decomposer, lineDecompose){
  Vector3 i,j,k; i=j=k=Vector3::Random_Unit();
  double a=10;

  Decomposer3d dec(i,j,k);
  Vector3 p = a*i;
  auto alpha = dec.Componenti(p),
    beta = dec.Componentj(p),
    gamma = dec.Componentk(p);
  EXPECT_TRUE(std::isnan(alpha));
  EXPECT_TRUE(std::isnan(beta));
  EXPECT_TRUE(std::isnan(gamma));

  p = Vector3::Cross(i, Vector3::Random_Unit()); // p is not parallel with i j nor k
  alpha = dec.Componenti(p); beta = dec.Componentj(p); gamma = dec.Componentk(p);
  EXPECT_TRUE(std::isnan(alpha));
  EXPECT_TRUE(std::isnan(beta));
  EXPECT_TRUE(std::isnan(gamma));
}
