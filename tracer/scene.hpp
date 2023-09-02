#ifndef SCENE_H
#define SCENE_H

#include "shape.hpp"
#include "bvh.hpp"

#include<iostream>
#include<memory>
#include<vector>

class Scene: public Shape{
public:
  Scene(){}
  Scene(std::shared_ptr<Shape> obj) : objects{obj} {
    bbox = obj->Get_Bounding_box();
  }
  Scene(const std::vector<std::shared_ptr<Shape>>& obj_src){
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
      Hit_record temp_rec,rec;
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

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override{
    return rec.hitted_obj->Ray_Color(r,rec);
  }

  virtual AABB Get_Bounding_box(void) const override{ return bbox; }

  std::string Get_Name(void) const override{ return std::string{"object_list"}; }
  void Set_material(std::shared_ptr<Material>) override{ std::cerr<<"Cannot set material to a scene"<<std::endl<<std::flush; }
  
protected:
  std::vector<std::shared_ptr<Shape>> objects;

  AABB bbox;
  std::shared_ptr<bvh_node> bv_tree;
};


#endif
