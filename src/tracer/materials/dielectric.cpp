#include "dielectric.hpp"

#include<bsdf.hpp>
#include<shape.hpp>

#include<cmath>

std::vector<std::shared_ptr<BxDF>> Dielectric::CalculateBSDF(const Hit_record& rec){
  class SpecularDielectric : public BxDF{
  public:
    SpecularDielectric(const Normal& norm,const double& e) : BxDF(BxDFType::Transmission|BxDFType::Specular), normal(norm),eta(e) {}

    Color f(const Vector3&, const Vector3&) const override{ return Color(0,0,0); }
    double pdf(const Vector3&,const Vector3&) const override{ return 0.0; }
    std::tuple<Color,Vector3,double> Sample_f(const Vector3& rin) const override{
      const double pdf = 1.0;
      const Vector3 out_direction = Get_out_direction(rin);
      return std::make_tuple(Color(1,1,1), out_direction, pdf);
    }
  private:
    Normal normal;
    double eta;

    double Reflectance(const double& cosine, const double& iof)const {
      auto r0 = (1-iof) / (1+iof);
      r0 = r0 * r0;
      return r0+(1-r0)*pow((1-cosine),5);
    }
    Vector3 Get_out_direction(const Vector3& in_direction)const {
      double cos_theta = fmin(Vector3::Dot(-in_direction, normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

      if(eta*sin_theta > 1.0 or Reflectance(cos_theta,eta)>random_uniform_01())
	return Ray(Point3(0,0,0), in_direction).Reflect_Direction(normal);
      else return Ray(Point3(0,0,0),in_direction).Refract_Direction(normal, eta);
    }
  };

  return std::vector<std::shared_ptr<BxDF>>{
    std::make_shared<SpecularDielectric>(rec.normal,
					 rec.front_face ? (1.0/eta) : eta)};
}
