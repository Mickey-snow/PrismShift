#include "pointlight.hpp"

#include<util/util.hpp>

std::shared_ptr<MockedBSDF> Pointlight::CalculateBSDF(const Hit_record& rec){
  class bsdf : public MockedBSDF{
  public:
    bsdf(Color _col) : col(_col) {}
    virtual bool doScatter() override{ return false; }
    virtual Vector3 ScatterDirection() override { return Vector3(0,0,0); }
    virtual Color Emission() override{ return col; }
    virtual Color f() override{ return Color(1,1,1); }
  private:
    Color col;
  };
  return std::make_shared<bsdf>(this->color);
}
