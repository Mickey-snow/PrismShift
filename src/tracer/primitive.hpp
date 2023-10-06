#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include<memory>

class IMaterial;
class Hit_record;
class IShape;
template<typename> class Interval;
class Ray;
class BSDF;
class AABB;

class Primitive{
public:
  Primitive();
  ~Primitive();

  Hit_record Hit(const Ray& r, const Interval<double>& t) const; 
  Primitive& Set_Shape(std::shared_ptr<IShape> shape){
    m_shape = shape;
    return *this;
  }
  AABB Get_Bbox(void);
  
  BSDF CalcBSDF(const Hit_record&) const;
  Primitive& Set_Material(std::shared_ptr<IMaterial> material){
    m_material = material;
    return *this;
  }

private:
  std::shared_ptr<IMaterial> m_material;
  std::shared_ptr<IShape> m_shape;
};




#endif
