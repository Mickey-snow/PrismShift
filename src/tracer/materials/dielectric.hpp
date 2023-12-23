// #ifndef DIELECTRIC_H
// #define DIELECTRIC_H

// #include<util/util.hpp>
// #include<material.hpp>
// #include<factory.hpp>
// #include<texture.hpp>

// #include<string>
// #include<memory>

// class Dielectric : public Material{
// public:
//   static constexpr std::string name{"dielectric"};

//   Dielectric(const double& index_of_refraction,const double& f=0) : eta(index_of_refraction),fuzz(fabs(f)){
//     assert(0<=fuzz && fuzz<=1);
//   }

//   virtual BSDF CalculateBSDF(const Hit_record&) override;

// private:
//   double eta,fuzz;
// };

// namespace{
// }



// #endif
