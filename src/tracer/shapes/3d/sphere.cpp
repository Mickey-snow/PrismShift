#include "sphere.hpp"
#include<material.hpp>
#include<shape.hpp>
#include<util/util.hpp>


#include<cmath>
#include<memory>


Sphere::Sphere() : material(nullptr), Visible() {}

Sphere& Sphere::Set_Position(const Point3& origin){
  refframe.Set_Translation(Coordinate3::Origin(origin));
  return *this;
}

Sphere& Sphere::Set_Radius(const double& r){
  refframe.Set_Scale(r,r,r);
  return *this;
}

Hit_record Sphere::Ray_Hit(const Ray& rp, const Interval<double>& time_interval) const{
  const Ray r = refframe.World2Local(rp);
  constexpr double radius = 1.0;
  
  Vector3 oc = (Vector3)r.Origin();
  auto a = r.Direction().Length_squared();
  auto half_b = Vector3::Dot(oc, r.Direction());
  auto c = oc.Length_squared() - radius*radius;

  auto discriminant = half_b*half_b - a*c;
  if (discriminant < 0) return Hit_record::NoHit();
  auto sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  auto root = (-half_b - sqrtd) / a;
  if (!time_interval.Surrounds(root)){
    root = (-half_b + sqrtd) / a;
    if (!time_interval.Surrounds(root))
      return Hit_record::NoHit();
  }

  double time = root;
  Point3 position = r.At(time);
  Normal normal = (Normal)position;
    
  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     r,
					     time,
					     position,
					     normal);
}


Point2 Sphere::Map_Texture(const Hit_record& rec) const{
  double x = rec.normal.x();
  double y = rec.normal.y();
  double z = rec.normal.z();

  double theta = acos(y);
  double phi = atan2(-z,x)+pi;

  double v = theta/pi;
  double u = phi/(2*pi);

  return Point2(u,v);
}
