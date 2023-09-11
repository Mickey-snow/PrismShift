#ifndef BSDF_H
#define BSDF_H

#include<util/util.hpp>

class BxDFType{
public:
  enum types{
    None = 0,
    Reflection = 1<<0,
    Transmission = 1<<1,
    Diffuse = 1<<2,
    Glossy, Specular
  };

  BxDFType(unsigned int _flag) : flag(_flag) {}

  bool operator == (const BxDFType& cp) const{ return (flag&cp.flag)!=0; }
  bool operator == (const unsigned int& cp) const{ return (flag&cp) != 0; }
private:
  unsigned int flag;
};


class BxDF{
public:
  BxDF(const BxDFType& _flag) : flag(_flag) {}
  virtual Color f(const Vector3&, const Vector3&)const = 0;
  virtual std::tuple<Color,Vector3,double> Sample_f(const Vector3& rin)const = 0;
  virtual double pdf(const Vector3& in_direction, const Vector3& out_direction)const = 0;

  bool MatchesFlag(unsigned int flag){ return this->flag == flag; }
  bool MatchesFlag(const BxDFType& flag){ return this->flag == flag; }
  BxDFType GetFlags(void){ return this->flag; }
private:
  BxDFType flag;
};


class BSDF{
private:
  static constexpr int MAX_BXDF_SIZE = 8;
public:
  BSDF(std::vector<std::shared_ptr<BxDF>> bxdf){
    bxdf_count = 0;
    for(auto it : bxdf) Add(it);
  }

  int bxdf_count;
  std::shared_ptr<BxDF> bxdfs[MAX_BXDF_SIZE];

  bool Add(std::shared_ptr<BxDF> bxdf){
    if(bxdf_count >= MAX_BXDF_SIZE) return false;
    bxdfs[bxdf_count++] = bxdf;
    return true;
  }

  virtual Color f(const Vector3&, const Vector3&) const;
  virtual std::tuple<Color,Vector3,double> Sample_f(const Vector3&) const;
  virtual double pdf(const Vector3&, const Vector3&) const;
};


class CosBxDF : public BxDF{
public:
  CosBxDF(const BxDFType& _flag, const Vector3& norm) : BxDF(_flag), normal(norm) {}

  virtual Color f(const Vector3&, const Vector3&) const override;
  virtual std::tuple<Color,Vector3,double> Sample_f(const Vector3& rin) const override;
  virtual double pdf(const Vector3&, const Vector3&) const override;
  
private:
  Vector3 normal;
};

#endif
