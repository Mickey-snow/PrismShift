#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include<util/util.hpp>
#include<material.hpp>
#include<factory.hpp>
#include<texture.hpp>
#include<textures/solidcolor.hpp>

#include<string>
#include<sstream>

class Lambertian : public Material{
public:
  static constexpr std::string name{"lambertian"};
  
  Lambertian(const Color& col){ texture = std::make_shared<SolidColor>(col); }
  Lambertian(std::shared_ptr<Texture> tex) : texture(tex) {}

  virtual Color Ray_Color(const Hit_record& rec) const override;
  
protected:
  std::shared_ptr<Texture> texture;
};

namespace{
  std::shared_ptr<Material> CreateLambertian(std::stringstream& ss){
    std::string texture_name; ss>>texture_name;
    auto texture = (TextureFactory::Instance()->GetCreateFn(texture_name))(ss);
    return std::make_shared<Lambertian>(texture);
  }
  constexpr std::string Lambertian_MaterialID = Lambertian::name;
  const bool lambertian_registered = MaterialFactory::Instance()->Register(Lambertian_MaterialID, CreateLambertian);
}

#endif
