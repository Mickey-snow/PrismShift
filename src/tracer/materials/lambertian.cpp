// #include "lambertian.hpp"

// #include<shape.hpp>
// #include<bsdf.hpp>
// #include<bxdfs/lambertian.hpp>
// #include<textures/solidcolor.hpp>

// Lambertian::Lambertian() : texture(nullptr) {}

// Lambertian& Lambertian::Set_Color(const Color& col){
//   texture = std::make_shared<SolidColor>(col);
//   return *this;
// }

// Lambertian& Lambertian::Set_Texture(std::shared_ptr<Texture> tex){
//   texture = tex;
//   return *this;
// }

// BSDF Lambertian::CalculateBSDF(const Hit_record& rec){
//   auto bxdf = std::make_shared<Lambertian_BRDF>(this->texture->ColorAt(rec.hitted_obj->Map_Texture(rec)));
//   return BSDF(bxdf)
//     .SetCoord(Coordinate3().Set_Translation(Coordinate3::Origin(rec.position)).Set_Rotation(Transformation::RotateFrTo(Vector3{0,0,1}, (Vector3)rec.normal)));
// }
