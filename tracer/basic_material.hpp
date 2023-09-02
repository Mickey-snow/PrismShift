#include "material.hpp"
#include "visual_shape.hpp"
#include "util/util.hpp"
#include "factory.hpp"

#include<string>
#include<memory>
#include<sstream>


class Lambertian : public Material{
public:
  static constexpr std::string name{"lambertian"};
  
  Lambertian(const Color& col) : attenuation(col) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
  
protected:
  Color attenuation;
};



class Metal : public Material{
public:
  static constexpr std::string name{"metal"};
  
  Metal(const Color& col, const double& f=0) : attenuation(col),fuzz(fabs(f)<1?f:1) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
  
protected:
  Color attenuation;
  double fuzz;
};



class Dielectric : public Material{
public:
  static constexpr std::string name{"dielectric"};
  
  Dielectric(const double& index_of_refraction) : eta(index_of_refraction) {}

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;
  
private:
  double eta;
};



namespace{
  std::shared_ptr<Material> CreateDielectric(std::stringstream& ss){
    double eta; ss>>eta;
    return std::make_shared<Dielectric>(eta);
  }
  constexpr std::string Dielectric_MaterialID = Dielectric::name;
  const bool registered1 = MaterialFactory::Instance()->Register(Dielectric_MaterialID, CreateDielectric);
}
namespace{
  std::shared_ptr<Material> CreateLambertian(std::stringstream& ss){
    double r,g,b; ss>>r>>g>>b;
    return std::make_shared<Lambertian>(Color(r,g,b));
  }
  constexpr std::string Lambertian_MaterialID = Lambertian::name;
  const bool registered2 = MaterialFactory::Instance()->Register(Lambertian_MaterialID, CreateLambertian);
}
namespace{
  std::shared_ptr<Material> CreateMetal(std::stringstream& ss){
    double r,g,b,fuzz; ss>>r>>g>>b>>fuzz;
    return std::make_shared<Metal>(Color(r,g,b), fuzz);
  }
  constexpr std::string Metal_MaterialID = Metal::name;
  const bool registered3 = MaterialFactory::Instance()->Register(Metal_MaterialID, CreateMetal);
}
