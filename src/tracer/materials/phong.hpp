#ifndef PHONG_H
#define PHONG_H

#include "metal.hpp"
#include "lambertian.hpp"

class Phong : public Material{
public:
  static constexpr std::string name{"phong"};

  Phong(const Color& ika, const Color& ikd, const Color& iks, const Color& ie, const double& ns) : emission(ie), shininess(ns), ambient(ika), diffuse(ikd), specular(iks) {}
  
  virtual Color Ray_Color(const Hit_record& rec) const override;

private:
  double shininess;
  Color emission;
  Color ambient,diffuse,specular;
};


namespace{
  std::shared_ptr<Material> CreatePhong(Json::Value attribute){
    Json::RequireMember(attribute, "ambient", "diffuse", "specular", "emission", "shininess");
    Color ka,kd,ks,e; double ns;

    auto ReadColor = [](Json::Value js){
      Color ret;
      for(int i=0;i<3;++i) ret[i] = js[i].asDouble();
      return ret;
    };

    ka = ReadColor(attribute["ambient"]);
    kd = ReadColor(attribute["diffuse"]);
    ks = ReadColor(attribute["specular"]);
    e = ReadColor(attribute["emission"]);
    ns = attribute["shininess"].asDouble();

    return std::make_shared<Phong>(ka,kd,ks,e,ns);
  }
  constexpr std::string Phong_MaterialID = Phong::name;
  const bool phong_registered = MaterialFactory::Instance()->Register(Phong_MaterialID, CreatePhong);
}


#endif
