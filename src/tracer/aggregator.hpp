#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include<util/util.hpp>
#include<shape.hpp>
#include<primitive.hpp>

#include<vector>
#include<memory>
#include<string>


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
