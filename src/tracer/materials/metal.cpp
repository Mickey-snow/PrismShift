#include "metal.hpp"

#include<bsdf.hpp>
#include<bxdfs/conductor.hpp>
#include<shape.hpp>

BSDF Metal::CalculateBSDF(const Hit_record& rec){
  auto bxdf = std::make_shared<Conductor_BRDF>(this->texture->ColorAt(rec.hitted_obj->Map_Texture(rec)), fuzz);

  return BSDF(bxdf)
    .SetCoord(Coordinate3().Set_Translation(Coordinate3::Origin(rec.position)).Set_Rotation(Transformation::RotateFrTo(Vector3{0,0,1}, (Vector3)rec.normal)));
}
