// #include "dielectric.hpp"

// #include<bsdf.hpp>
// #include<shape.hpp>
// #include<util/util.hpp>
// #include<bxdfs/dielectric.hpp>

// #include<cmath>

  

// BSDF Dielectric::CalculateBSDF(const Hit_record& rec){
//   auto bxdf = std::make_shared<DielectricBTDF>(rec.front_face ? (1.0/eta) : eta, fuzz);
//   return BSDF(bxdf)
//     .SetCoord(Coordinate3().Set_Translation(Coordinate3::Origin(rec.position)).Set_Rotation(Transformation::RotateFrTo(Vector3{0,0,1}, (Vector3)rec.normal)));
// }
