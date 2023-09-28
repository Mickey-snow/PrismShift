#ifndef SPHERE_H
#define SPHERE_H

#include<material.hpp>
#include<shape.hpp>
#include<util/util.hpp>
#include<factory.hpp>

#include<cmath>
#include<memory>
#include<sstream>
#include<string>

class Sphere : public Visible{
public:
  static constexpr std::string name{"sphere"};

  Sphere();

  Sphere& Set_Position(const Point3&);
  Sphere& Set_Radius(const double&);
  
  AABB Get_Bounding_box(void) const override{
    return refframe.Local2World(AABB(Point3(1,1,1), Point3(-1,-1,-1)));
  }

  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;

  virtual Point2 Map_Texture(const Hit_record& rec) const override;

  
  std::string Get_Name(void) const override{ return name; }
  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void)const override{ return material; }
  
private:
  std::shared_ptr<Material> material;
};

namespace{
  std::shared_ptr<Visible> CreateSphere(const std::vector<Attribute>& attributes){
    auto ret = std::make_shared<Sphere>();

    for(const auto& attr : attributes)
      if(attr.name == "position")
	ret->Set_Position(std::any_cast<Point3>(attr.val));
      else if(attr.name == "radius")
	ret->Set_Radius(std::any_cast<double>(attr.val));
      else if(attr.name == "material")
	ret->Set_Material(std::any_cast<std::shared_ptr<Material>>(attr.val));
      else std::cerr<<"at CreateSphere: Unknown attribute "<<attr.name<<std::endl;
      
    return ret;
  }

  constexpr std::string Sphere_ShapeID = Sphere::name;
  const bool sphere_registered = ShapeFactory::Instance()->Register(Sphere_ShapeID, CreateSphere);
}

#endif
