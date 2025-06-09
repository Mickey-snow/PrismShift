// #ifndef DIELECTRIC_BTDF_H
// #define DIELECTRIC_BTDF_H

// #include<bsdf.hpp>

// class Vector3;
// class Color;

// class DielectricBTDF : public BxDF{
// public:
//   DielectricBTDF(const double& e,const double& f=0) : eta(e),fuzz(f) {
//     isSpecular = fuzz==0;

//     unsigned int flag = BxDFType::Transmission;
//     if(eta != 1) flag |= BxDFType::Reflection;

//     flag |= (isSpecular ? BxDFType::Specular : BxDFType::Glossy);
//     BxDF::SetFlags(flag);
//   }

//   Color f(const Vector3& wi, const Vector3& wo) const override;
//   double pdf(const Vector3& wi,const Vector3& wo) const override;
//   bxdfSample Sample_f(const Vector3& rin) const override;

// private:
//   double eta,fuzz;
//   bool isSpecular;

//   double Reflectance(const double& cosine, const double& iof)const;
//   Vector3 Get_out_direction(const Vector3& in_direction)const;
// };

// #endif
