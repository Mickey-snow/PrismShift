#include "scene_factory.hpp"

#include "material.hpp"
#include "shapes/shape.hpp"
#include "texture.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

/* -- helpers ------------------------------------------------------------ */

Camera SceneFactory::parse_camera(const json& r) {
  Point3 center(r.at(0), r.at(1), r.at(2));
  Point3 lookat(r.at(3), r.at(4), r.at(5));
  int image_h = r.at(6);
  double aspect = r.at(7);
  double vFovDeg = r.at(8);
  return Camera(center, lookat, image_h, aspect, vFovDeg);
}

/* -- ctor / loader ------------------------------------------------------ */

SceneFactory::SceneFactory(json root) : root_(std::move(root)) {
  parse_materials(root_.at("materials"));
}

SceneFactory SceneFactory::FromFile(const std::string& path) {
  json root;
  std::ifstream ifs(path);
  if (!ifs)
    throw std::runtime_error("cannot open scene file: " + path);

  spdlog::info("loading scene from file: '{}' ...", path);
  ifs >> root;
  return SceneFactory(std::move(root));
}

/* -- materials ---------------------------------------------------------- */

void SceneFactory::parse_materials(const json& array) {
  for (const auto& it : array) {
    const std::string type = it.at("type");
    json v = it.at("data");

    std::shared_ptr<Material> mat;
    if (type == "conductor") {
      mat = std::make_shared<Material>(
          std::make_shared<SolidColor>(v.at(0), v.at(1), v.at(2)),
          std::make_shared<FloatConst>(v.at(3)));
    } else if (type == "diffuse") {
      mat = std::make_shared<Material>(
          std::make_shared<SolidColor>(v.at(0), v.at(1), v.at(2)), nullptr);
    } else if (type == "dielectric") {
      if (v.is_array())
        v = v.at(0);
      mat =
          std::make_shared<Material>(nullptr, std::make_shared<FloatConst>(v));
    } else {
      spdlog::error("unknown material {}", type);
      continue;
    }
    materials_.emplace_back(std::move(mat));
  }
}

/* -- primitives --------------------------------------------------------- */

std::shared_ptr<Primitive> SceneFactory::make_sphere(std::size_t matId,
                                                     Point3 pos,
                                                     double d) const {
  static const auto sphere = std::make_shared<Sphere>();

  std::shared_ptr<Material> mat = nullptr;
  if (matId < materials_.size())
    mat = materials_[matId];
  else
    spdlog::error("material id ({}) out of bounds.", matId);

  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  return std::make_shared<Primitive>(sphere, std::move(mat), std::move(trans));
}

/* -- scene -------------------------------------------------------------- */

Scene SceneFactory::parse_scene(const json& array) const {
  std::vector<std::shared_ptr<Primitive>> objs;

  for (const auto& it : array) {
    const std::string type = it.at("type");
    const json& v = it.at("data");

    if (type == "sphere") {
      objs.emplace_back(
          make_sphere(v.at(0), Point3(v.at(1), v.at(2), v.at(3)), v.at(4)));
    } else if (type == "plane") {
      objs.emplace_back(make_2d<Plane>(v.at(0),
                                       Point3(v.at(1), v.at(2), v.at(3)),
                                       Point3(v.at(4), v.at(5), v.at(6)),
                                       Point3(v.at(7), v.at(8), v.at(9))));
    } else if (type == "triangle") {
      objs.emplace_back(make_2d<Triangle>(v.at(0),
                                          Point3(v.at(1), v.at(2), v.at(3)),
                                          Point3(v.at(4), v.at(5), v.at(6)),
                                          Point3(v.at(7), v.at(8), v.at(9))));
    } else if (type == "quad") {
      objs.emplace_back(
          make_2d<Parallelogram>(v.at(0), Point3(v.at(1), v.at(2), v.at(3)),
                                 Point3(v.at(4), v.at(5), v.at(6)),
                                 Point3(v.at(7), v.at(8), v.at(9))));
    } else {
      spdlog::error("unsupported shape: {}", type);
    }
  }
  return Scene(std::move(objs));
}

/* -- public builders ---------------------------------------------------- */

Camera SceneFactory::CreateCamera() const {
  return parse_camera(root_.at("camera"));
}

Scene SceneFactory::CreateScene() const {
  return parse_scene(root_.at("objects"));
}
