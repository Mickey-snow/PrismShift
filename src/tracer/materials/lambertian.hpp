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
  std::shared_ptr<Material> CreateLambertian(Json::Value attribute){
    if(attribute.isMember("texture")){
	Json::Value texture_attr = attribute["texture"];
	std::string texture_type = texture_attr["type"].asString();

	auto texture = (TextureFactory::Instance()->GetCreateFn(texture_type))(texture_attr["attribute"]);
	return std::make_shared<Lambertian>(texture);
    } else {
      Json::RequireMember(attribute, "rgb");
      double r = attribute["rgb"][0].asDouble();
      double g = attribute["rgb"][1].asDouble();
      double b = attribute["rgb"][2].asDouble();
      return std::make_shared<Lambertian>(Color(r,g,b));
    }
  }
  constexpr std::string Lambertian_MaterialID = Lambertian::name;
  const bool lambertian_registered = MaterialFactory::Instance()->Register(Lambertian_MaterialID, CreateLambertian);
}

#endif
