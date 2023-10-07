#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include<util/util.hpp>
#include<shape.hpp>
#include<primitive.hpp>

#include<vector>
#include<memory>
#include<string>

class bvh_node : public Shape{
  // A bvh node class, representing a subtree from a KDT
  // It holds a AABB Bounding Box property which fully encloses all objects in the KDT
  // Concrete visible objects are at leaves
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

  [[deprecated]]
  AABB Get_Bounding_box(void) const override{ return bbox; }
  AABB Get_Bbox(void) const { return bbox; }
  
  virtual std::string Get_Name(void) const override{
    return std::string{"BVH Node"};
  }
private:
  std::shared_ptr<Shape> lch,rch;
  AABB bbox;
};




class IAggregate{
public:
  virtual ~IAggregate() = default;

  virtual AABB Get_Bbox(void) const = 0;
  virtual Hit_record Hit(const Ray&, const Interval<double>&) const = 0;
};

class BVT : public IAggregate{
private:
  class Node : public IShape{
  public:
    Node(const std::vector<std::shared_ptr<IPrimitive>>& src){
      std::vector<std::shared_ptr<IPrimitive>> src_obj_copy(src);
      *this = Node(src_obj_copy, 0, src_obj_copy.size(), 0);
    }
    Node(std::vector<std::shared_ptr<IPrimitive>>& src_obj, size_t start, size_t end, int axis=0);
    
    Hit_record Hit(const Ray&,const Interval<double>&)const;
    AABB Get_Bbox(void) const { return bbox; }
    
  private:
    std::shared_ptr<IShape> lch,rch;
    AABB bbox;
    friend BVT;
  };
  
public:
  BVT(const std::vector<std::shared_ptr<IPrimitive>>& li);

  AABB Get_Bbox(void) const override{ return bbox; }
  Hit_record Hit(const Ray&, const Interval<double>&) const override;
  
private:
  AABB bbox;
  std::unique_ptr<Node> root;
};


#endif
