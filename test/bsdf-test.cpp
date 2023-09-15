#include<bsdf.hpp>

#include<gtest/gtest.h>
#include "constant.hpp"

constexpr int samples = 240000;
constexpr double LEPS = 0.1;

TEST(cosbxdf, pdf){
  Normal normal(0,1,0);
  CosBxDF bxdf(0,normal);
  
  double sum = 0;
  for(int i=0;i<samples;++i){
    double theta = pi*i/2/samples;
    Vector3 out(sin(theta),cos(theta),0);
    sum += bxdf.pdf(Vector3::Random_Unit(), out) * pi * (pi/2 / samples);
  }

  EXPECT_NEAR(sum, 1, LEPS);
}

TEST(cosbxdf, samplefMedian){
  Normal normal(0,1,0);
  CosBxDF bxdf(0,normal);

  const double cos_median_val = cos(pi/4);
  int lesshalf=0,greaterhalf=0;
  for(int i=0;i<samples;++i){
    auto [f,out,pdf] = bxdf.Sample_f(Vector3::Random_Unit());
    double costheta = Vector3::Dot(out,normal)/(normal.Length()*out.Length());

    if(costheta > cos_median_val) ++lesshalf;
    else ++greaterhalf;
  }

  EXPECT_NEAR(lesshalf,greaterhalf,samples/100);
}

TEST(cosbxdf, samplefMean){
  const double mean_val = pi/4;
  Normal normal = (Normal)Vector3::Random_Unit();
  CosBxDF bxdf(0,normal);

  double sum = 0;
  for(int i=0;i<samples;++i){
    auto [f,out,pdf] = bxdf.Sample_f(Vector3::Random_Unit());
    double costheta = Vector3::Dot(out,normal)/(normal.Length()*out.Length());

    sum += acos(costheta);
  }

  EXPECT_NEAR(sum/samples, mean_val, LEPS);
}

TEST(cosbxdf, integrateByCosDistribution){
  auto f_val = [](double alpha, double theta){
    return cos(theta) * sin(alpha);
  };
  CosBxDF bxdf(0,Normal(0,1,0));

  double sum = 0;
  for(int i=0;i<samples;++i){
    auto [f, w, pdf] = bxdf.Sample_f(Vector3::Random_Unit());
    w = w.Normalize();
    double alpha = atan(w.z()/w.x()), theta = acos(w.y());
    sum += f_val(alpha,theta) / pdf;
  }

  EXPECT_NEAR(sum/samples, 0, LEPS);
}


TEST(bsdftest, integrateByCombinedBxdf){
  class RandomDirectionBxDF : public BxDF{
  public:
    using BxDF::BxDF;
    Color f(const Vector3&, const Vector3&) const override { return Color(0,0,0); }
    std::tuple<Color,Vector3,double> Sample_f(const Vector3&) const override{
      auto direction = Vector3::Random_Unit();
      if(direction[1] < 0) direction[1] = -direction[1];
      return std::make_tuple(f(dummy,dummy), direction, pdf(dummy,dummy));
    }
    double pdf(const Vector3&, const Vector3&) const override{ return 1.0/(2*pi); }
  private:
    Vector3 dummy = Vector3(0,0,0);
  };
  
  auto f_val = [](double alpha, double theta){
    return exp(alpha) / tan(theta);
  };
  const double expect_int_f_val = 2*(exp(pi/2)-exp(-pi/2));

  auto bsdf = BSDF(std::vector<std::shared_ptr<BxDF>>{
      std::make_shared<CosBxDF>(BxDFType::Diffuse|BxDFType::Reflection, Normal(0,1,0)),
      std::make_shared<RandomDirectionBxDF>(BxDFType::Diffuse|BxDFType::Reflection)});
  double sum = 0;
  for(int i=0;i<samples;++i){
    auto [f,w,pdf,flags] = bsdf.Sample_f(Vector3::Random_Unit());
    double alpha = atan(w.z()/w.x()), theta = acos(w.y());

    pdf = bsdf.pdf(Vector3::Random_Unit(),w) / bsdf.bxdf_count;
    sum += f_val(alpha,theta) / pdf;
  }

  EXPECT_NEAR(sum/samples, expect_int_f_val, LEPS);
}

TEST(bsdftest, intergrateWithSpecialBxdf){
  class StaticSamplerBxDF : public BxDF{
  public:
    using BxDF::BxDF;
    Color f(const Vector3&, const Vector3&) const override{ return Color(0,0,0); }
    std::tuple<Color,Vector3,double> Sample_f(const Vector3&) const override{
      return std::make_tuple(f(dummy,dummy),
			     Vector3(1,1,1).Normalize(),
			     1.0);
    }
    double pdf(const Vector3&, const Vector3&) const override{ return 0.0; }
  private:
    Vector3 dummy = Vector3(0,0,0);
  };

  auto f_val = [](double alpha, double theta){
    return std::log(fabs(theta)+2*pi) * std::exp(alpha*alpha);
  };
  const double expect_int_f_val = 26.6735636;

  auto bsdf = BSDF(std::vector<std::shared_ptr<BxDF>>{
      std::make_shared<CosBxDF>(BxDFType::Diffuse|BxDFType::Reflection, Normal(0,1,0)),
      std::make_shared<CosBxDF>(BxDFType::Diffuse|BxDFType::Reflection, Normal(0,1,0)),
      std::make_shared<StaticSamplerBxDF>(BxDFType::Specular|BxDFType::Reflection)});
  double sum = 0;
  for(int i=0;i<samples;++i){
    auto [f,w,pdf,flags] = bsdf.Sample_f(Vector3::Random_Unit());
    if((flags & BxDFType::Specular) == BxDFType::None){
      pdf = bsdf.pdf(Vector3::Random_Unit(),w,BxDFType::Diffuse)/2;
    }
    
    double alpha = atan(w.z()/w.x()), theta = acos(w.y());
    sum += f_val(alpha,theta) / pdf;
  }
  
  EXPECT_NEAR(sum/samples,expect_int_f_val,LEPS*5);
}
