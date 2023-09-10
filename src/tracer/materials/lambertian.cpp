#include "lambertian.hpp"

#include<util/util.hpp>
#include<renderer.hpp>
#include<shape.hpp>

std::shared_ptr<MockedBSDF> Lambertian::CalculateBSDF(const Hit_record& rec){
  class bsdf : public MockedBSDF{
  public:
    bsdf(Ray _rin, Vector3 _norm, Color _col) : rin(_rin), normal(_norm), col(_col) {}
    virtual bool doScatter() override{ return true; }
    virtual Vector3 ScatterDirection() override {
      return rin.Scatter_Direction(normal);
    }
    virtual Color Emission() override{ return Color(0,0,0); }
    virtual Color f() override{ return col; }
  private:
    Ray rin;
    Color col;
    Vector3 normal;
  };
  return std::make_shared<bsdf>(rec.ray, rec.normal, this->texture->ColorAt(rec.hitted_obj->Map_Texture(rec)));
}
