#ifndef BSDF_H
#define BSDF_H

#include<util/util.hpp>
#include<utility>
#include<memory>

class BxDFType{
public:
  enum types{
    None = 0,
    Reflection = 1<<0,
    Transmission = 1<<1,
    Diffuse = 1<<2,
    Glossy = 1<<3,
    Specular = 1<<4,
    ALL = Reflection|Transmission|Diffuse|Glossy|Specular
  };

  BxDFType() : flag(0) {}
  BxDFType(unsigned int _flag) : flag(_flag) {}

  BxDFType operator & (const BxDFType& cp) const{ return BxDFType(flag&cp.flag); }
  BxDFType operator & (const unsigned int& cp) const{ return BxDFType(flag&cp); }
  bool operator == (const BxDFType& cp) const { return flag == cp.flag; }
  bool operator == (const unsigned int& cp) const { return flag == cp; }
  bool operator != (const BxDFType& cp) const { return !(*this == cp); }
  bool operator != (const unsigned int& cp) const { return !(*this == cp); }
private:
  unsigned int flag;
};

class BxDF;
struct bxdfSample{
  Color col;
  Vector3 out_direction;
  double pdf;
  BxDF const* bxdf;

  bxdfSample() : col{0,0,0},out_direction{0,0,0},pdf(0.0),bxdf(nullptr) {}
  bxdfSample(const Color& _col, const Vector3& _rout, const double& _pdf, BxDF const* _bxdf) :
    col(_col), out_direction(_rout), pdf(_pdf), bxdf(_bxdf) {}
};


class BxDF{
public:
  BxDF(){}
  BxDF(const BxDFType& _flag) : flag(_flag) {}
  
  // returns the value of the distribution function for the given pair of directions
  virtual Color f(const Vector3&, const Vector3&)const = 0;

  // a method that uses importance sampling to draw a direction from a distribution that approximately matches the scattering function's shape
  virtual bxdfSample Sample_f(const Vector3& rin)const = 0;

  // returns the value of the probability density function for the given pair of directions
  virtual double pdf(const Vector3& in_direction, const Vector3& out_direction)const = 0;

  bool MatchesFlag(unsigned int flag)const{ return (this->flag&flag)!=0; }
  bool MatchesFlag(const BxDFType& flag) const { return (this->flag&flag)!=0; }
  BxDFType GetFlags(void) const { return this->flag; }

private:
  BxDFType flag;

protected:
  template<typename... Ts>
  bxdfSample Make_Sample(Ts&&... params) const{
    return bxdfSample(std::forward<Ts>(params)...);
  }

  template<typename T>
  BxDF& SetFlags(T&& _flag){ flag=BxDFType(std::forward<T>(_flag)); return *this; }
};


class BSDF{
public:
  BSDF(){ bxdf=nullptr; }
  BSDF(std::shared_ptr<BxDF> _bxdf) : bxdf(std::move(_bxdf)) {}
  
  
  BSDF& SetBxdf(std::shared_ptr<BxDF> _bxdf){
    bxdf = _bxdf;
    return *this;
  }
  template<typename T>
  BSDF& SetCoord(T&& coord){
    frame = std::forward<T>(coord);
    return *this;
  }

  virtual Color f(const Vector3&, const Vector3&,const BxDFType& flag=BxDFType::ALL) const;
  virtual bxdfSample Sample_f(const Vector3&) const;
  virtual double pdf(const Vector3&, const Vector3&,const BxDFType& flag=BxDFType::ALL) const;

private:
  std::shared_ptr<BxDF> bxdf;
  Coordinate3 frame;


public:
  BSDF(const BSDF& rhs) : frame(rhs.frame),
			  bxdf(rhs.bxdf)  {}
  BSDF& operator = (const BSDF& rhs){
    frame=rhs.frame;
    bxdf=rhs.bxdf;
    return *this;
  }
  BSDF(BSDF&& rhs) : frame(std::move(rhs.frame)), bxdf(std::move(rhs.bxdf)) {}
  BSDF& operator = (BSDF&& rhs){ frame=std::move(rhs.frame); bxdf=std::move(rhs.bxdf); return *this; }
};


#endif
