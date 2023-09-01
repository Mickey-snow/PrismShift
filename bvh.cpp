#include "bvh.hpp"

#include<stdexcept>
#include<format>
#include<memory>

const Interval<double>& AABB::Axis(const int& n) const{
    if(n == 0) return x_interval;
    if(n == 1) return y_interval;
    if(n == 2) return z_interval;
    throw std::range_error(std::format("Range Error: AABB::Axis(n) accepts n=0,1,2 for x,y,z interval components. given n={}", n));
}

bool AABB::Is_Hit_in_Interval(const Ray& r, Interval<double> ray_t)const {
  for(int a=0;a<3;++a){
    auto invD = 1 / r.Direction()[a];
    auto orig = r.Origin()[a];

    auto t0 = (Axis(a).begin - orig) * invD;
    auto t1 = (Axis(a).end - orig) * invD;

    if(invD < 0) std::swap(t0,t1);

    // Overlap the (a) component's interval [t0,t1]
    // Let ray_t be the new effective time interval
    if(t0 > ray_t.begin) ray_t.begin = t0;
    if(t1 < ray_t.end) ray_t.end = t1;

    if(ray_t.end <= ray_t.begin) return false;
  }
  return true;
}



Color bvh_node::Ray_Color(const Ray& r, const Hit_record& rec) const{
  throw std::runtime_error("bvh_node::Ray_Color should never be called.");
  return Color(0,0,0);
}

// Construct a bvh tree
bvh_node::bvh_node(std::vector<std::shared_ptr<Visible>>& src_obj, size_t start, size_t end, int axis){
  class __final_comparer{
  public:
    __final_comparer(const AABB::Componentbased_Comparer& cp) : component_comparer(cp) {}
    bool operator () (std::shared_ptr<Visible> a, std::shared_ptr<Visible> b){
      return component_comparer(a->Get_Bounding_box(), b->Get_Bounding_box());
    }
  private:
    AABB::Componentbased_Comparer component_comparer;
  };
  
  auto comparer = __final_comparer(AABB::Componentbased_Comparer(axis));
  size_t object_span = end-start;

  if(object_span == 1) lch = rch = src_obj[start];
  else if(object_span == 2){
    if(comparer(src_obj[start], src_obj[start+1])){
      lch = src_obj[start]; rch = src_obj[start+1];
    } else {
      lch = src_obj[start+1]; rch = src_obj[start];
    }
  }
  else if(object_span >= 3){
    size_t mid = start + object_span/2;
    std::nth_element(src_obj.begin()+start, src_obj.begin()+mid, src_obj.begin()+end, comparer);
    int next_axis = (axis+1)%3;

    lch = std::make_shared<bvh_node>(src_obj, start, mid, next_axis);
    rch = std::make_shared<bvh_node>(src_obj, mid, end, next_axis);
  } else throw std::runtime_error(std::format("Array offset error while constructing bvt. start={}, end={}, span={}", start,end,object_span));

  bbox = AABB(lch->Get_Bounding_box(), rch->Get_Bounding_box());
}


// Only provide answer to Ray_Hit queries if it is certain that the ray missed
Hit_record bvh_node::Ray_Hit(const Ray& r, const Interval<double>& time) const{
  Hit_record rec; rec.hits = false;
  double closest_hit_time = time.end;
  
  if(not bbox.Is_Hit_in_Interval(r,time)) return rec; // missed

  // Deffer query to children
  Hit_record temp_rec = lch->Ray_Hit(r,time);
  if(temp_rec.hits){
    rec = temp_rec;
    closest_hit_time = temp_rec.time;
  }

  temp_rec = rch->Ray_Hit(r,Interval{time.begin, closest_hit_time});
  if(temp_rec.hits) rec = temp_rec;

  return rec;
}

