// #ifndef METAL_H
// #define METAL_H

// #include<util/util.hpp>
// #include<material.hpp>
// #include<factory.hpp>
// #include<texture.hpp>
// #include<textures/solidcolor.hpp>

// #include<string>
// #include<memory>

// class Metal : public Material{
// public:
//   static constexpr std::string name{"metal"};

//   Metal(const Color& col) : Metal(std::make_shared<SolidColor>(col),0.0) {}
//   Metal(std::shared_ptr<Texture> tex) : Metal(tex, 0.0) {}
//   Metal(std::shared_ptr<Texture> tex, const double& f) : texture(tex) {
//   fuzz=fabs(f)>1?1.0:fabs(f); }

//   virtual BSDF CalculateBSDF(const Hit_record&) override;

// private:
//   std::shared_ptr<Texture> texture;
//   double fuzz;
// };

// namespace {
// }

// #endif
