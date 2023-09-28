#ifndef PLANE_H
#define PLANE_H

#include<util/util.hpp>
#include<shape.hpp>
#include<material.hpp>
#include<factory.hpp>

#include<utility>
#include<memory>
#include<string>
#include<sstream>


class Plane : public Visible{
public:
  static constexpr std::string name{"plane"};
  std::string Get_Name(void) const override{ return name; }

  Plane();
  Plane& Set_Position(const Point3&, const Point3&, const Point3&);

  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void)const override{ return material; }
  
  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;

  virtual Point2 Map_Texture(const Hit_record&) const override;

protected:
  AABB bbox;

  std::shared_ptr<Material> material;
};

namespace{
  std::shared_ptr<Visible> CreatePlane(const std::vector<Attribute>& attributes){
    auto ret = std::make_shared<Plane>();

    for(const auto& attr : attributes)
      if(attr.name == "material")
	ret->Set_Material(std::any_cast<std::shared_ptr<Material>>(attr.val));
      else if(attr.name == "position"){
	auto v = std::any_cast<std::array<Point3,3>>(attr.val);
	ret->Set_Position(v[0],v[1],v[2]);
      }
      else std::cerr<<"at CreatePlane: Unknown attribute "<<attr.name<<std::endl;
	
    
    return ret;
  }

  constexpr std::string Plane_ShapeID = Plane::name;
  const bool plane_registered = ShapeFactory::Instance()->Register(Plane_ShapeID, CreatePlane);
}

#endif
