#ifndef BVF_H
#define BVF_H

#include<util/util.hpp>
#include<shape.hpp>
#include<material.hpp>

#include<vector>
#include<memory>
#include<iostream>


class bvh_node : public Shape{
  // A bvh node class, representing a subtree from a KDT
  // It holds a AABB Bounding Box property which fully encloses all objects in the KDT
  // Colored visible objects are at leaves
public:
  bvh_node(const std::vector<std::shared_ptr<Shape>>& src){
    std::vector<std::shared_ptr<Shape>> src_obj_copy(src);
    *this = bvh_node(src_obj_copy, 0, src_obj_copy.size(), 0);
  }
  bvh_node(std::vector<std::shared_ptr<Shape>>& src_obj, size_t start, size_t end, int axis=0);
  // Constructing a bvh will require to modify the array src_obj

  // If a Ray misses hitting a bounding box, it misses all objects in the subtree
  // If not, deffer the query to its children
  // However it never set itself as the hitted object
  Hit_record Ray_Hit(const Ray&,const Interval<double>&)const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual std::string Get_Name(void) const override{
    return std::string{"BVH Node"};
  }
private:
  std::shared_ptr<Shape> lch,rch;
  AABB bbox;
};

#endif
