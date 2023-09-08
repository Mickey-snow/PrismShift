#ifndef IMAGE_TEX_H
#define IMAGE_TEX_H


#include<util/util.hpp>
#include<factory.hpp>
#include<texture.hpp>

#include<memory>
#include<string>
#include<sstream>


class ImageTexture : public Texture{
public:
  static constexpr std::string name{"imagetex"};

  ImageTexture(std::string filename);

  virtual Color ColorAt(const Point2&) override;
  
private:
  cv::Mat img;
};
namespace{
  std::shared_ptr<Texture> CreateImageTexture(Json::Value attribute){
    Json::RequireMember(attribute, "path");
    std::string filename = attribute["path"].asString();
    return std::make_shared<ImageTexture>(filename);
  }
  constexpr std::string Imagetex_TextureID = ImageTexture::name;
  const bool imagetex_registered = TextureFactory::Instance()->Register(Imagetex_TextureID, CreateImageTexture);
}

#endif
