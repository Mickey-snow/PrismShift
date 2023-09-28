#ifndef TRIANGLE_H
#define TRIANGEL_H

#include<util/util.hpp>
#include<factory.hpp>
#include<shape.hpp>

#include<memory>
#include<string>
#include<sstream>
#include<array>


class Triangle : public Visible{
public:
  static constexpr std::string name{"triangle"};
  virtual std::string Get_Name(void) const override{ return name; }
  
  Triangle();
  
  Triangle& Set_Position(const Point3&, const Point3&, const Point3&);
  Triangle& Set_Normals(const Normal&, const Normal&, const Normal&);


  virtual Hit_record Ray_Hit(const Ray&, const Interval<double>&) const override;
  Point2 Map_Texture(const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }
  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void) const override{ return material; }
  
protected:
  AABB bbox;
  Normal nu,nv,nw;
  
  std::shared_ptr<Material> material;
};




namespace{
  std::shared_ptr<Visible> CreateTriangle(const std::vector<Attribute>& attributes){
    auto ret = std::make_shared<Triangle>();

    for(const auto& attr : attributes)
      if(attr.name == "position"){
	auto v = std::any_cast<std::array<Point3,3>>(attr.val);
	ret->Set_Position(v[0], v[1], v[2]);
      }
      else if(attr.name == "normal"){
	auto v = std::any_cast<std::array<Normal,3>>(attr.val);
	ret->Set_Normals(v[0],v[1],v[2]);
      }
      else std::cerr<<"at CreateTriangle: Unknown attribute "<<attr.name<<std::endl;
    
    return ret;
  }

  constexpr std::string Triangle_ShapeID = Triangle::name;
  const bool triangle_registered = ShapeFactory::Instance()->Register(Triangle_ShapeID, CreateTriangle);
}


#endif
