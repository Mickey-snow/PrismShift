#include "metal.hpp"

#include<bsdf.hpp>
#include<shape.hpp>

std::vector<std::shared_ptr<BxDF>> Metal::CalculateBSDF(const Hit_record& rec){
  class SpecularMetal : public BxDF{
  public:
    SpecularMetal(const Vector3& norm, const Color& col) : BxDF(BxDFType::Reflection|BxDFType::Specular),normal(norm),color(col) {}

    Color f(const Vector3&,const Vector3&) const override{ return Color(0,0,0); }
    std::tuple<Color,Vector3,double> Sample_f(const Vector3& rin) const override{
      const double pdf = 1.0;
      const Vector3 out_direction = Ray(Vector3(0,0,0), rin).Reflect_Direction(normal);
      return std::make_tuple(color, out_direction, pdf);
    }
    double pdf(const Vector3&,const Vector3&) const override{ return 0.0; }
    
  private:
    Vector3 normal;
    Color color;
  };

  if(fuzz < 0.03)
    return std::vector<std::shared_ptr<BxDF>>{
      std::make_shared<SpecularMetal>(rec.normal,
			     this->texture->ColorAt(rec.hitted_obj->Map_Texture(rec)))};
  
  else assert(false); // implement this later
}
