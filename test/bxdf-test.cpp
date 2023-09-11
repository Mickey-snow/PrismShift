#include<bsdf.hpp>

#include<gtest/gtest.h>
#include "constant.hpp"
#include "vec-comparer.hpp"


TEST(cosbxdf, pdf){
  Vector3 normal(0,1,0);
  CosBxDF bxdf(0,normal);
  
  constexpr int samples = 10000;
  double sum = 0;
  for(int i=0;i<samples;++i){
    double theta = pi*i/2/samples;
    Vector3 out(sin(theta),cos(theta),0);
    sum += bxdf.pdf(Vector3::Random_Unit(), out) * pi * (pi/2 / samples);
  }

  EXPECT_NEAR(sum, 1, 1e-3);
}

TEST(cosbxdf, samplef){
  constexpr int samples = 100000;
  Vector3 normal = Vector3(0,1,0);
  CosBxDF bxdf(0,normal);

  int lesshalf=0,greaterhalf=0;
  for(int i=0;i<samples;++i){
    auto [f,out,pdf] = bxdf.Sample_f(Vector3::Random_Unit());
    double costheta = Vector3::Dot(out,normal)/(normal.Length()*out.Length());
    if(costheta > cos(pi/4)) ++lesshalf;
    else ++greaterhalf;
  }

  EXPECT_NEAR(lesshalf,greaterhalf,samples/90);
}

TEST(cosbxdf, integrateByCosDistribution){
  auto f_val = [](double alpha, double theta){
    return cos(theta) * sin(alpha);
  };
  CosBxDF bxdf(0,Vector3(0,1,0));

  constexpr auto samples = 100000;
  double sum = 0;
  for(int i=0;i<samples;++i){
    auto [f, w, pdf] = bxdf.Sample_f(Vector3::Random_Unit());
    w = w.Unit();
    double alpha = acos(w.x()), theta = acos(w.y());
    sum += f_val(alpha,theta) / pdf;
  }

  EXPECT_NEAR(sum/samples, 2, 1e-2);
}
