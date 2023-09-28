#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include<util/util.hpp>
#include<shape.hpp>
#include<factory.hpp>

#include<memory>
#include<string>
#include<sstream>


class Parallelogram : public Visible{
public:
  static constexpr std::string name{"parallelogram"};
  std::string Get_Name(void) const override{ return name; }

  Parallelogram();
  
  Parallelogram& Set_Position(const Point3&, const Point3&, const Point3&);
  
  virtual Hit_record Ray_Hit(const Ray&, const Interval<double>&) const override;
  Point2 Map_Texture(const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }
  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void)const override{ return material; }

protected:
  AABB bbox;

  std::shared_ptr<Material> material;
};

namespace{
  std::shared_ptr<Visible> CreateParallelogram(const std::vector<Attribute>& attributes){
    auto ret = std::make_shared<Parallelogram>();

    for(const auto& attr : attributes)
      if(attr.name == "position"){
	auto v = std::any_cast<std::array<Point3, 3>>(attr.val);
	ret->Set_Position(v[0],v[1],v[2]);
      }
      else if(attr.name == "material")
	ret->Set_Material(std::any_cast<std::shared_ptr<Material>>(attr.val));
      else std::cerr<<"at CreateParallelogram: Unknown attribute "<<attr.name<<std::endl;
    
    return ret;
  }

  constexpr std::string Parallelogram_ShapeID = Parallelogram::name;
  const bool parallelogram_registered = ShapeFactory::Instance()->Register(Parallelogram_ShapeID, CreateParallelogram);
}


#endif
