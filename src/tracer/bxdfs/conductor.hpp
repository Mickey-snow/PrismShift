#ifndef CONDUCTOR_BRDF_H
#define CONDUCTOR_BRDF_H

#include<bsdf.hpp>

class Vector3;

class Conductor_BRDF : public BxDF{
public:
  Conductor_BRDF(const Color& _col, const double& f=0):
    isSpecular(fabs(f)<1e-6), fuzz(fabs(f)>1?1:fabs(f)), col(_col){
    unsigned int flag = BxDFType::Reflection;
    if(isSpecular) flag |= BxDFType::Specular;
    else flag |= BxDFType::Glossy;
    BxDF::SetFlags(flag);
  }

  Color f(const Vector3&, const Vector3&) const override;
  bxdfSample Sample_f(const Vector3&) const override;
  double pdf(const Vector3&, const Vector3&) const override;

  
private:
  bool isSpecular;
  double fuzz;
  Color col;

  Vector3 Reflected_Direction(const Vector3&) const;
};



#endif
