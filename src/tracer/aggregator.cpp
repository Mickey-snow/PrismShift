#include "aggregator.hpp"

#include<util/util.hpp>
#include<shape.hpp>
#include<primitive.hpp>

#include<stdexcept>
#include<format>
#include<memory>


BVT::BVT(const std::vector<std::shared_ptr<IPrimitive>>& li){
  root = std::make_unique<Node>(li);
  bbox = root->Get_Bbox();
}

Hit_record BVT::Hit(const Ray& r, const Interval<double>& time_interval) const {
  return root->Hit(r,time_interval);
}


BVT::Node::Node(std::vector<std::shared_ptr<IPrimitive>>& src_obj, size_t start, size_t end, int axis){
  class __final_comparer{
  public:
    __final_comparer(const AABB::Componentbased_Comparer& cp) : component_comparer(cp) {}
    bool operator () (std::shared_ptr<IPrimitive> a, std::shared_ptr<IPrimitive> b){
      return component_comparer(a->Get_Bbox(), b->Get_Bbox());
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

    lch = std::make_shared<Node>(src_obj, start, mid, next_axis);
    rch = std::make_shared<Node>(src_obj, mid, end, next_axis);
  } else throw std::runtime_error(std::format("Array offset error while constructing bvt. start={}, end={}, span={}", start,end,object_span));

  bbox = AABB{lch->Get_Bbox(), rch->Get_Bbox()};
}


Hit_record BVT::Node::Hit(const Ray& r, const Interval<double>& time) const{
  double closest_hit_time = time.end;
  
  if(not bbox.isHitIn(r,time)) return Hit_record{};
  Hit_record rec = Hit_record{};
  
  Hit_record temp_rec = lch->Hit(r,time);
  if(temp_rec.isHit()){
    rec = temp_rec;
    closest_hit_time = temp_rec.time;
  }

  temp_rec = rch->Hit(r,Interval{time.begin, closest_hit_time});
  if(temp_rec.isHit()) rec = temp_rec;

  return rec;
}
