#ifndef SOLIDCOLOR_H
#define SOLIDCOLOR_H


#include<texture.hpp>
#include<factory.hpp>

#include<memory>
#include<string>
#include<vector>

class Point2;

class SolidColor : public Texture{
public:
  static constexpr std::string name{"solidcolor"};

  SolidColor();
  SolidColor(const Color& col){ Set_Color(col); }
  SolidColor(const double& r, const double& g, const double& b){ Set_Color(r,g,b); }
  
  SolidColor& Set_Color(const Color&);
  SolidColor& Set_Color(const double&, const double&, const double&);

  virtual Color ColorAt(const Point2&) override;
private:
  Color color;
};

namespace{
  std::shared_ptr<Texture> CreateSolidColor(const std::vector<Attribute>& attributes){
    auto ret = std::make_shared<SolidColor>();

    for(const auto& attr : attributes)
      if(attr.name == "rgb"){
	try{
	  auto rgb_arr = std::any_cast<std::array<double,3>>(attr.val);
	  ret->Set_Color(rgb_arr[0], rgb_arr[1], rgb_arr[2]);
	  continue;
	} catch(std::bad_cast& e){}
	try{
	  auto col = std::any_cast<Color>(attr.val);
	  ret->Set_Color(col);
	  continue;
	} catch(std::bad_cast& e){}
	throw std::runtime_error("at CreateSolidColor: value type for attribute rgb is unknown");
      }

    return ret;
  }
  
  constexpr std::string SolidColor_TextureID = SolidColor::name;
  const bool solidcolor_registered = TextureFactory::Instance()->Register(SolidColor_TextureID, CreateSolidColor);
}

#endif
