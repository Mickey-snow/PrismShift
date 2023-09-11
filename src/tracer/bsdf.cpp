#include "bsdf.hpp"


Color CosBxDF::f(const Vector3& in_direction, const Vector3& out_direction) const{
  return Color(1,1,1);
}

std::tuple<Color,Vector3,double> CosBxDF::Sample_f(const Vector3& in_direction) const{
  Vector3 a = (fabs(normal.x()) > 0.9) ? Vector3(0,1,0) : Vector3(1,0,0);
  Vector3 v = Vector3::Cross(normal,a).Unit();
  Vector3 u = Vector3::Cross(normal,v);

  double r1 = random_uniform_01(), r2 = random_uniform_01();
  double phi = r1*2*pi;
  double x = cos(phi)*std::sqrt(r2);
  double y = sin(phi)*std::sqrt(r2);
  double z = std::sqrt(1-r2);

  Vector3 out_direction =  x*u+y*v+z*normal;
  return std::make_tuple(f(in_direction, out_direction),
			 out_direction,
			 pdf(in_direction,out_direction));
}

double CosBxDF::pdf(const Vector3& in_direction, const Vector3& out_direction) const{
  double dot_product = Vector3::Dot(normal, out_direction);
  if(dot_product < 0) return 0;
  double costheta = dot_product / (normal.Length() * out_direction.Length());
  return costheta / pi;
}



Color BSDF::f(const Vector3& rin, const Vector3& rout) const{
  Color col(1,1,1);
  std::for_each(bxdfs,bxdfs+bxdf_count,
		[&col,&rin,&rout](const std::shared_ptr<BxDF>& bxdf){
		  col *= bxdf->f(rin,rout); });
  return col;
}

std::tuple<Color, Vector3, double> BSDF::Sample_f(const Vector3& rin) const{
  int chosen_bxdf_id = random_uniform_01() * bxdf_count;
  BxDFType chosen_flags = bxdfs[chosen_bxdf_id]->GetFlags();

  return bxdfs[chosen_bxdf_id]->Sample_f(rin);
}

double BSDF::pdf(const Vector3& rin, const Vector3& rout) const{
  double pdf = 0;
  std::for_each(bxdfs,bxdfs+bxdf_count,
		[&pdf,&rin,&rout](const std::shared_ptr<BxDF>& bxdf){
		  pdf += bxdf->pdf(rin,rout);});
  return pdf;
}
