// #include "dielectric.hpp"
// #include<util/util.hpp>

// #include<cmath>

// Color DielectricBTDF::f(const Vector3& wi, const Vector3& wo) const{
//   return isSpecular ? Color(0,0,0) : Color(1,1,1);
// }

// double DielectricBTDF::pdf(const Vector3& wi,const Vector3& wo) const{
//   if(isSpecular) return 0.0;

//   auto delta_w = wo - Get_out_direction(wi);
//   if(delta_w.Length_squared() < fuzz*fuzz+1e-6)
//     return 0.25*invpi;
//   return 0.0;
// }

// bxdfSample DielectricBTDF::Sample_f(const Vector3& rin) const{
//   const Vector3 out_direction = Get_out_direction(rin);

//   if(isSpecular)
//     return BxDF::Make_Sample(Color(1,1,1), out_direction, 1.0,this);
//   else{
//     const Vector3 delta_w = Vector3::Random_Unit();
//     return BxDF::Make_Sample(Color(1,1,1), out_direction+fuzz*delta_w,
//     0.25*invpi, this);
//   }
// }

// double DielectricBTDF::Reflectance(const double& cosine, const double&
// iof)const {
//   auto r0 = (1-iof) / (1+iof);
//   r0 = r0 * r0;
//   return r0+(1-r0)*pow((1-cosine),5);
// }
// Vector3 DielectricBTDF::Get_out_direction(const Vector3& in_direction)const {
//   const Normal normal{0,0,1};
//   double cos_theta = fmin(Vector3::Dot(-in_direction, normal), 1.0);
//   double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

//   if(eta*sin_theta > 1.0 or Reflectance(cos_theta,eta)>random_uniform_01())
//     return Ray(Point3(0,0,0), in_direction).Reflect_Direction(normal);
//   else return Ray(Point3(0,0,0),in_direction).Refract_Direction(normal, eta);
// }
