#include "parallelogram.hpp"

#include<util/util.hpp>
#include<utility>

Color Parallelogram::Ray_Color(const Ray &r, const Hit_record &rec) const{
  if(material == nullptr) return Color(0,0,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Parallelogram::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  Hit_record rec_miss,rec; rec_miss.hits = false;
  
  auto [hitsplane, time] = plane->Intersection(r, time_interval);
  auto intersection = r.At(time);
  if(not hitsplane) return rec_miss;

  auto [alpha, beta] = plane->Decomposition(intersection-Q);
  if(!On_Object(alpha,beta)) return rec_miss;

  rec.hits = true;
  rec.hitted_obj = Get_ptr();
  rec.time = time;
  rec.position = intersection;
  rec.Set_Face_Normal(r, plane->normal);

  return rec;
}

Point2 Parallelogram::Map_Texture(const Ray& r, const Hit_record& rec) const{
  return plane->Map_Texture(r,rec);
}