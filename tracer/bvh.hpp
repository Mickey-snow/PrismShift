#ifndef BVF_H
#define BVF_H

#include "util/util.hpp"
#include "visual_shape.hpp"

#include<vector>
#include<memory>
#include<iostream>

class AABB{
  // Approximate an visible object to a cube
  // All rays miss this cube thus miss the object
public:
  AABB():x_interval(Interval<double>::Empty()), y_interval(Interval<double>::Empty()), z_interval(Interval<double>::Empty()) {}
  AABB(const Point3& a, const Point3& b){
    x_interval = Interval(fmin(a.x(),b.x()), fmax(a.x(),b.x()));
    y_interval = Interval(fmin(a.y(),b.y()), fmax(a.y(),b.y()));
    z_interval = Interval(fmin(a.z(),b.z()), fmax(a.z(),b.z()));
  }
  AABB(const AABB& a,const AABB& b):
    x_interval(Interval(a.x_interval,b.x_interval)),
    y_interval(Interval(a.y_interval,b.y_interval)),
    z_interval(Interval(a.z_interval,b.z_interval)) {}

  bool operator == (const AABB& rhs) const{
    return x_interval == rhs.Axis(0) &&
      y_interval == rhs.Axis(1) &&
      z_interval == rhs.Axis(2);
  }
  
  const Interval<double>& Axis(const int& n) const;

  bool Is_Hit_in_Interval(const Ray&, Interval<double>) const;

  class Componentbased_Comparer{
  public:
    Componentbased_Comparer(const int& _component) : component(_component) {}
    bool operator () (const AABB& a, const AABB& b) const{
      return a.Axis(component).begin < b.Axis(component).begin;
    }
  private:
    int component;
  };
private:
  Interval<double> x_interval,y_interval,z_interval;
};

class bvh_node : public Visible{
  // A bvh node class, representing a subtree from a KDT
  // It holds a AABB Bounding Box property which fully encloses all objects in the KDT
  // Colored visible objects are at leaves
public:
  bvh_node(const std::vector<std::shared_ptr<Visible>>& src){
    std::vector<std::shared_ptr<Visible>> src_obj_copy(src);
    *this = bvh_node(src_obj_copy, 0, src_obj_copy.size(), 0);
  }
  bvh_node(std::vector<std::shared_ptr<Visible>>& src_obj, size_t start, size_t end, int axis=0);
  // Constructing a bvh will require to modify the array src_obj

  // If a Ray misses hitting a bounding box, it misses all objects in the subtree
  // If not, deffer the query to its children
  // However it never set itself as the hitted object
  Hit_record Ray_Hit(const Ray&,const Interval<double>&)const override;

  // This method should never be called since a structural bch node will never be the hitted object in the Hit_record
  Color Ray_Color(const Ray&, const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }

  std::string Get_Name(void) const override{ return std::string{"bvh_node"}; }
  void Set_material(std::shared_ptr<Material>) override{ std::cerr<<"Cannot set material to a bvh node"<<std::endl<<std::flush; }
private:
  std::shared_ptr<Visible> lch,rch;
  AABB bbox;
};

#endif
