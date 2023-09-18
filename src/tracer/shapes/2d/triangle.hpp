#ifndef TRIANGLE_H
#define TRIANGEL_H

#include<util/util.hpp>
#include<factory.hpp>
#include<shape.hpp>

#include<memory>
#include<string>
#include<sstream>
#include<array>

#include "plane.hpp"

class Triangle : public Visible{
public:
  static constexpr std::string name{"triangle"};
  std::string Get_Name(void) const override{ return name; }

  Triangle(const Point3& _Q, const Vector3& _u, const Vector3& _v) :
    Q(_Q),u(_u),v(_v),
    Visible(Coordinate3().Set_Translation(Coordinate3::Origin(_Q)).Set_Rotation(Coordinate3::AlignXY(_u,_v))){
    has_normal=false; Init(); }
  Triangle(const std::array<Point3,3>& p,const std::array<Normal,3>& n){
    Q = p[0]; u = p[1]-p[0]; v = p[2]-p[0];
    refframe = Coordinate3().Set_Translation(Coordinate3::Origin(Q)).Set_Rotation(Coordinate3::AlignXY(u,v));
    nw = n[0]; nu = n[1]; nv = n[2];
    has_normal=true; Init();
  }
  
  static Triangle* From_points(const Point3& a,const Point3& b,const Point3& c){ return new Triangle(a,b-a,c-a); }
  static Triangle* From_pointvecvec(const Point3& a,const Vector3& u,const Vector3& v){ return new Triangle(a,u,v); }
  static Triangle* From_points_normals(const std::array<Point3,3> p, const std::array<Normal, 3> norm){
    return new Triangle(p,norm);
  }

  virtual Hit_record Ray_Hit(const Ray&, const Interval<double>&) const override;
  Point2 Map_Texture(const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }
  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void) override{ return material; }
protected:
  AABB bbox;
  Point3 Q;
  Vector3 u,v;
  Normal nu,nv,nw;
  bool has_normal;
  
  std::shared_ptr<Material> material;
  Decomposer3d* planeDec;
  Decomposer3d* normalDec;
  
  void Init(void){
    Normal normal = (Normal)(Vector3::Normalized(Vector3::Cross(u,v)));
    if(!has_normal) nu=nw=nv=normal;
    
    bbox = AABB(AABB(Q,Q+u),AABB(Q,Q+v)).Pad();
    
    planeDec = new Decomposer3d(u,v,normal);
    normalDec = new Decomposer3d(Q,Q+u,Q+v);
  }

  bool On_Object(const double& a,const double& b) const{
    return (a+b)<=1 && 0<=a && 0<=b;
  }
};




namespace{
  std::shared_ptr<Visible> CreateTriangle(Json::Value attribute){
    Json::RequireMember(attribute, "vertex");
    Point3 u,v,w;
    auto wi=attribute["vertex"][0], ui=attribute["vertex"][1], vi=attribute["vertex"][2];

    u =Point3(ui[0].asDouble(), ui[1].asDouble(), ui[2].asDouble());
    v = Point3(vi[0].asDouble(), vi[1].asDouble(), vi[2].asDouble());
    w = Point3(wi[0].asDouble(), wi[1].asDouble(), wi[2].asDouble());

    
    if(attribute.isMember("normal")){
      Normal nv,nu,nw;
      wi=attribute["normal"][0], ui=attribute["normal"][1], vi=attribute["normal"][2];

      nu =Normal(ui[0].asDouble(), ui[1].asDouble(), ui[2].asDouble());
      nv = Normal(vi[0].asDouble(), vi[1].asDouble(), vi[2].asDouble());
      nw = Normal(wi[0].asDouble(), wi[1].asDouble(), wi[2].asDouble());
      
      return std::shared_ptr<Triangle>(Triangle::From_points_normals(std::array{w,u,v},
									std::array{nw,nu,nv}
									));
    }
    else return std::shared_ptr<Triangle>(Triangle::From_points(w,u,v));
  }

  constexpr std::string Triangle_ShapeID = Triangle::name;
  const bool triangle_registered = ShapeFactory::Instance()->Register(Triangle_ShapeID, CreateTriangle);
}


#endif
