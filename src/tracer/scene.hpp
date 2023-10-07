#ifndef SCENE_H
#define SCENE_H


#include<iostream>
#include<memory>
#include<vector>

#include<shape.hpp>
#include<aggregator.hpp>



class Scene: public Shape{
public:
  Scene(){}
  Scene(std::shared_ptr<Shape> obj) : objects{obj} {
    bbox = obj->Get_Bounding_box();
  }
  Scene(const std::vector<std::shared_ptr<Shape>>& obj_src){
    for(auto obj : obj_src) Add(obj);
  }
  Scene(const std::vector<std::shared_ptr<Visible>>& obj_src){
    for(auto obj : obj_src) Add(obj);
  }
  ~Scene() = default;
  
  void Clear(){ objects.clear(); bbox=AABB(); bv_tree=nullptr; }
  void Add(std::shared_ptr<Shape> obj){
    objects.push_back(obj);
    bbox = AABB(bbox, obj->Get_Bounding_box());
    bv_tree = nullptr;
  }

  void Build_BVH(void){
    std::clog<<"\rBuilding BVH"<<std::endl;
    bv_tree = std::make_shared<bvh_node>(objects);
  }

  // returns true iff Ray r hits any object in the scene
  // Hit_record rec perscribes the first object Ray r hits, i.e., with the smallest time t>=time.begin
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override{
    if(bv_tree != nullptr) return bv_tree->Ray_Hit(r,time);

    else{
      Hit_record temp_rec=Hit_record::NoHit(),rec=Hit_record::NoHit();
      double closest_hit_time = time.end;

      for(const auto& obj : objects){
	temp_rec = obj->Ray_Hit(r, Interval{time.begin, closest_hit_time});
	if(temp_rec.hits){
	  rec = temp_rec;
	  closest_hit_time = temp_rec.time;
	}
      }

      return rec;
    }
  }


  virtual AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual std::string Get_Name(void) const override{ return std::string{"Object List"}; }
  
protected:
  std::vector<std::shared_ptr<Shape>> objects;

  AABB bbox;
  std::shared_ptr<bvh_node> bv_tree;
};


class IAggregate;
class Primitive;
class AABB;

class PrimitiveList : IShape{
  PrimitiveList() : m_shapes{}, m_aggregator(nullptr) {}
  ~PrimitiveList() = default;

  Hit_record Hit(const Ray&, const Interval<double>&) const override;
  AABB Get_Bbox(void) const override;

  PrimitiveList& Add(const std::shared_ptr<IPrimitive> shape);

  
private:
  std::vector<std::shared_ptr<IPrimitive>> m_shapes;
  mutable std::unique_ptr<IAggregate> m_aggregator;

  void Make_Aggregator(void) const;

};

#endif
