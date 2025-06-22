#include "texture.hpp"

#include <Imath/ImathBox.h>
#include <OpenEXR/ImfRgbaFile.h>  // OpenEXR high-level RGBA helper

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace {
// image texture loaders
static std::vector<float> load_ldr(const char* file, int& w, int& h, int& c) {
  stbi_set_flip_vertically_on_load(true);
  unsigned char* pix = stbi_load(file, &w, &h, &c, 0);
  if (!pix)
    return {};
  std::vector<float> buf;
  buf.resize(size_t(w) * h * c);
  for (size_t i = 0; i < buf.size(); ++i)
    buf[i] = pix[i] * (1.0f / 255.0f);
  stbi_image_free(pix);
  return buf;
}
std::vector<float> load_exr(const char* file, int& w, int& h, int& c) {
  using namespace OPENEXR_IMF_NAMESPACE;
  using namespace IMATH_NAMESPACE;

  RgbaInputFile in(file);
  Box2i dw = in.dataWindow();  // pixel rectangle
  w = dw.max.x - dw.min.x + 1;
  h = dw.max.y - dw.min.y + 1;
  c = 4;  // we will always output RGBA

  std::vector<Rgba> pixels(size_t(w) * h);  // half-precision RGBA
  in.setFrameBuffer(pixels.data() - dw.min.x - dw.min.y * w, 1, w);
  in.readPixels(dw.min.y, dw.max.y);

  // convert half → float, drop A if you prefer
  std::vector<float> buf;
  buf.resize(size_t(w) * h * c);
  for (size_t i = 0; i < pixels.size(); ++i) {
    const Rgba& p = pixels[i];
    buf[i * 4 + 0] = p.r;
    buf[i * 4 + 1] = p.g;
    buf[i * 4 + 2] = p.b;
    buf[i * 4 + 3] = p.a;  // keep alpha (1.0 if none)
  }
  return buf;
}
}  // namespace

ImageTexture::ImageTexture(std::vector<float> data, int w, int h, int c)
    : data_(std::move(data)), w_(w), h_(h), ch_(c) {}

std::shared_ptr<ImageTexture> ImageTexture::Create(std::filesystem::path path) {
  const std::string ext = path.extension();

  int w, h, ch;
  std::vector<float> data;

  if (ext == ".exr")
    data = load_exr(path.string().c_str(), w, h, ch);
  else
    data = load_ldr(path.string().c_str(), w, h, ch);

  if (data.empty())
    throw std::runtime_error("Failed loading texture: " + path.string());
  if (ch < 3)
    throw std::runtime_error("Texture must have at least 3 channels");

  return std::make_shared<ImageTexture>(std::move(data), w, h, ch);
}

Color ImageTexture::Evaluate(Point2 uv) const {
  Float x = uv.x() * (w_ - 1);
  Float y = uv.y() * (h_ - 1);

  int x0 = int(x);
  int y0 = int(y);
  int x1 = std::min(x0 + 1, w_ - 1);
  int y1 = std::min(y0 + 1, h_ - 1);
  Float tx = x - x0;
  Float ty = y - y0;

  Color c00 = Texel(x0, y0);
  Color c10 = Texel(x1, y0);
  Color c01 = Texel(x0, y1);
  Color c11 = Texel(x1, y1);

  Color c0 = Color::Lerp(c00, c10, tx);
  Color c1 = Color::Lerp(c01, c11, tx);
  return Color::Lerp(c0, c1, ty);
}

Color ImageTexture::Texel(int x, int y) const {
  const float* p = &data_[(y * w_ + x) * ch_];
  return Color(p[0], p[1], p[2]);
}
