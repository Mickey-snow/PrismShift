#ifndef SOLIDCOLOR_H
#define SOLIDCOLOR_H

#include<util/util.hpp>
#include<texture.hpp>
#include<factory.hpp>

#include<memory>

class SolidColor : public Texture{
public:
  static constexpr std::string name{"solidcolor"};
  
  SolidColor(const Color& c) : color(c) {}
  SolidColor(double r,double g,double b) : color(r,g,b) {}

  virtual Color ColorAt(const Point2&) override;
private:
  Color color;
};

namespace{
  std::shared_ptr<Texture> CreateSolidColor(Json::Value attribute){
    Json::RequireMember(attribute, "rgb");
    double r = attribute["rgb"][0].asDouble();
    double g = attribute["rgb"][1].asDouble();
    double b = attribute["rgb"][2].asDouble();
    return std::make_shared<SolidColor>(r,g,b);
  }
  
  constexpr std::string SolidColor_TextureID = SolidColor::name;
  const bool solidcolor_registered = TextureFactory::Instance()->Register(SolidColor_TextureID, CreateSolidColor);
}

#endif
