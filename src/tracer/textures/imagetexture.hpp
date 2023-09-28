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

  ImageTexture();

  ImageTexture& Set_Imgfile(std::string filename);

  virtual Color ColorAt(const Point2&) override;
  
private:
  cv::Mat img;
};


namespace{
  std::shared_ptr<Texture> CreateImageTexture(const std::vector<Attribute>& attributes){
    std::shared_ptr<ImageTexture> tex = std::make_shared<ImageTexture>();
    for(const auto& attr : attributes)
      if(attr.name == "path")
	tex->Set_Imgfile(std::any_cast<std::string>(attr.val));
      else std::cerr<<"at CreateImageTexture: Unknown attribute "<<attr.name<<std::endl;
    return tex;
  }
  
  constexpr std::string Imagetex_TextureID = ImageTexture::name;
  const bool imagetex_registered = TextureFactory::Instance()->Register(Imagetex_TextureID, CreateImageTexture);
}

#endif
