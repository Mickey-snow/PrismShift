#ifndef SCENE_H
#define SCENE_H

#include "objects.hpp"

#include<memory>
#include<vector>

class Scene: public Visible{
public:
  Scene(){}
  Scene(std::shared_ptr<Visible> obj) : objects{obj} {}
  Scene(std::initializer_list<std::shared_ptr<Visible>>&& li):objects{std::move(li)} {}
  ~Scene() = default;
  
  void Clear(){ objects.clear(); }
  void Add(std::shared_ptr<Visible> obj){ objects.push_back(obj); }

  // returns true iff Ray r hits any object in the scene
  // Hit_record rec perscribes the first object Ray r hits, i.e., with the smallest time t>=time.begin
  virtual Hit_record Ray_Hit(const Ray& r, Interval time) const override{
    Hit_record temp_rec,rec; rec.hits = false;
    double closest_hit_time = time.end;

    for(const auto& obj : objects){
      temp_rec = obj->Ray_Hit(r, Interval{time.begin+1e-6, closest_hit_time});
      if(temp_rec.hits){
	rec = temp_rec;
	closest_hit_time = temp_rec.time;
      }
    }

    return hit_any;
  }

  virtual Color Ray_color(const Ray& r, const Hit_record& rec) const override{
    return rec.hitted_obj->Ray_color(r,rec);
  }
  
protected:
  std::vector<std::shared_ptr<Visible>> objects;
};


#endif
