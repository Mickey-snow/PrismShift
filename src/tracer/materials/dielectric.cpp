#include "dielectric.hpp"

#include<bsdf.hpp>
#include<shape.hpp>
#include<util/util.hpp>

#include<cmath>


class DielectricBTDF : public BxDF{
public:
  DielectricBTDF(const double& e,const double& f=0) : eta(e),fuzz(f) {
    isSpecular = fuzz==0;
    
    unsigned int flag = BxDFType::Transmission;
    if(eta != 1) flag |= BxDFType::Reflection;

    flag |= (isSpecular ? BxDFType::Specular : BxDFType::Glossy);
    BxDF::SetFlags(flag);
  }

  Color f(const Vector3& wi, const Vector3& wo) const override{
    return isSpecular ? Color(0,0,0) : Color(1,1,1);
  }
  double pdf(const Vector3& wi,const Vector3& wo) const override{
    if(isSpecular) return 0.0;

    auto delta_w = wo - Get_out_direction(wi);
    if(delta_w.Length_squared() < fuzz*fuzz+1e-6)
      return 0.25*invpi;
    return 0.0;
  }
  bxdfSample Sample_f(const Vector3& rin) const override{
    const Vector3 out_direction = Get_out_direction(rin);

    if(isSpecular)
      return BxDF::Make_Sample(Color(1,1,1), out_direction, 1.0,this);
    else{
      const Vector3 delta_w = Vector3::Random_Unit();
      return BxDF::Make_Sample(Color(1,1,1), out_direction+fuzz*delta_w, 0.25*invpi, this);
    }
  }

private:
  double eta,fuzz;
  bool isSpecular;
  
  double Reflectance(const double& cosine, const double& iof)const {
    auto r0 = (1-iof) / (1+iof);
    r0 = r0 * r0;
    return r0+(1-r0)*pow((1-cosine),5);
  }
  Vector3 Get_out_direction(const Vector3& in_direction)const {
    const Normal normal{0,0,1};
    double cos_theta = fmin(Vector3::Dot(-in_direction, normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    if(eta*sin_theta > 1.0 or Reflectance(cos_theta,eta)>random_uniform_01())
      return Ray(Point3(0,0,0), in_direction).Reflect_Direction(normal);
    else return Ray(Point3(0,0,0),in_direction).Refract_Direction(normal, eta);
  }
};
  

BSDF Dielectric::CalculateBSDF(const Hit_record& rec){
  auto bxdf = std::make_shared<DielectricBTDF>(rec.front_face ? (1.0/eta) : eta, fuzz);
  return BSDF(bxdf)
    .SetCoord(Coordinate3().Set_Translation(Coordinate3::Origin(rec.position)).Set_Rotation(Transformation::RotateFrTo(Vector3{0,0,1}, (Vector3)rec.normal)));
}
