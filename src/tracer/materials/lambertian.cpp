#include "lambertian.hpp"

#include<util/util.hpp>
#include<renderer.hpp>
#include<shape.hpp>
#include<bsdf.hpp>

std::vector<std::shared_ptr<BxDF>> Lambertian::CalculateBSDF(const Hit_record& rec){
  class bsdf : public CosBxDF{
  public:
    bsdf(unsigned int type, Vector3 normal, Color color) : CosBxDF(type,normal),col(color) {}

    Color f(const Vector3&, const Vector3&) const override{ return col; }
  private:
    Color col;
  };

  return std::vector<std::shared_ptr<BxDF>>{
    std::make_shared<bsdf>(BxDFType::Glossy|BxDFType::Diffuse,
				rec.normal,
				this->texture->ColorAt(rec.hitted_obj->Map_Texture(rec)))
  };
}
