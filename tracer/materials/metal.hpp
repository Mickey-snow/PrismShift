#ifndef METAL_H
#define METAL_H

#include<material.hpp>
#include<util/util.hpp>
#include<factory.hpp>
#include<texture.hpp>
#include<textures/solidcolor.hpp>

#include<string>
#include<sstream>

class Metal : public Material{
public:
  static constexpr std::string name{"metal"};
  
  Metal(const Color& col, const double& f=0) : fuzz(fabs(f)<1?f:1) { texture = std::make_shared<SolidColor>(col); }
  Metal(std::shared_ptr<Texture> tex, const double& f=0) : fuzz(fabs(f)<1?f:1), texture(tex) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
  
protected:
  std::shared_ptr<Texture> texture;
  double fuzz;
};

namespace{
  std::shared_ptr<Material> CreateMetal(std::stringstream& ss){
    double fuzz; ss>>fuzz;
    std::string texture_name; ss>>texture_name;
    auto texture = (TextureFactory::Instance()->GetCreateFn(texture_name))(ss);
    return std::make_shared<Metal>(texture, fuzz);
  }
  constexpr std::string Metal_MaterialID = Metal::name;
  const bool metal_registered = MaterialFactory::Instance()->Register(Metal_MaterialID, CreateMetal);
}


#endif
