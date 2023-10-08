// #ifndef LAMBERTIAN_H
// #define LAMBERTIAN_H

// #include<material.hpp>
// #include<factory.hpp>

// #include<string>
// #include<memory>

// class Hit_record;
// class Texture;
// class BSDF;

// class Lambertian : public Material{
// public:
//   static constexpr std::string name{"lambertian"};

//   Lambertian();

//   Lambertian& Set_Color(const Color&);
//   Lambertian& Set_Texture(std::shared_ptr<Texture>);

//   virtual BSDF CalculateBSDF(const Hit_record&) override;
  
// protected:
//   std::shared_ptr<Texture> texture;
// };

// namespace{
//   std::shared_ptr<Material> CreateLambertian(const std::vector<Attribute>& attributes){
//     auto ret = std::make_shared<Lambertian>();

//     for(const auto& attr : attributes)
//       if(attr.name == "rgb")
// 	ret->Set_Color(std::any_cast<Color>(attr.val));
//       else if(attr.name == "texture")
// 	ret->Set_Texture(std::any_cast<std::shared_ptr<Texture>>(attr.val));
    
//     return ret;
//   }
  
//   constexpr std::string Lambertian_MaterialID = Lambertian::name;
//   const bool lambertian_registered = MaterialFactory::Instance()->Register(Lambertian_MaterialID, CreateLambertian);
// }

// #endif
