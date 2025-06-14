#include "scene_factory.hpp"

#include "light.hpp"
#include "material.hpp"
#include "shapes/shape.hpp"
#include "texture.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

/* -- helpers ------------------------------------------------------------ */

inline static Point3 parse_point3(auto it) {
  return Point3(it[0], it[1], it[2]);
}

Camera SceneFactory::parse_camera(const json& r) {
  Point3 center(parse_point3(r.cbegin()));
  Point3 lookat(parse_point3(r.cbegin() + 3));
  int image_h = r.at(6);
  double aspect = r.at(7);
  double vFovDeg = r.at(8);
  return Camera(center, lookat, image_h, aspect, vFovDeg);
}

/* -- ctor / loader ------------------------------------------------------ */

SceneFactory::SceneFactory(json root) : root_(std::move(root)) {
  if (root_.contains("materials"))
    parse_materials(root_.at("materials"));
  if (root_.contains("lights"))
    parse_lights(root_.at("lights"));
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
  materials_.reserve(array.size());
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

    if (it.contains("name"))
      material_map_[it.at("name")] = mat;
    materials_.emplace_back(std::move(mat));
  }
}

/* -- lights --------------------------------------------------------- */
void SceneFactory::parse_lights(const json& r) {
  lights_.reserve(r.size());
  for (const auto& it : r) {
    const std::string type = it.at("type");
    auto& v = it.at("data");
    auto light = std::make_shared<Light>(Color(v.at(0), v.at(1), v.at(2)));

    if (it.contains("name"))
      light_map_[it.at("name")] = light;
    lights_.emplace_back(std::move(light));
  }
}

/* -- primitives --------------------------------------------------------- */

void SceneFactory::add_sphere(Point3 pos,
                              double d,
                              std::shared_ptr<Material> mat,
                              std::shared_ptr<ILight> light) {
  static const auto sphere = std::make_shared<Sphere>();

  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  objs_.emplace_back(std::make_shared<Primitive>(
      sphere, std::move(mat), std::move(light), std::move(trans)));
}

void SceneFactory::add_cube(Point3 o,
                            Point3 a,
                            Point3 b,
                            Point3 c,
                            std::shared_ptr<Material> mat,
                            std::shared_ptr<ILight> light) {
  Vector3 e1 = a - o, e2 = b - o, e3 = c - o;

  add_2d<Parallelogram>(o, a, b, mat, light);
  add_2d<Parallelogram>(o, a, c, mat, light);
  add_2d<Parallelogram>(o, b, c, mat, light);

  auto o2 = o + e1 + e2 + e3;
  add_2d<Parallelogram>(o2, o2 - e1, o2 - e2, mat, light);
  add_2d<Parallelogram>(o2, o2 - e1, o2 - e3, mat, light);
  add_2d<Parallelogram>(o2, o2 - e2, o2 - e3, mat, light);
}

/* -- scene -------------------------------------------------------------- */

Scene SceneFactory::parse_objects(const json& array) {
  objs_.clear();
  objs_.reserve(array.size());

  for (const auto& it : array) {
    const std::string type = it.at("type");
    const json& v = it.at("data");

    std::shared_ptr<Material> mat = nullptr;
    std::shared_ptr<ILight> light = nullptr;
    if (it.contains("material"))
      mat = materials_[it.at("material")];
    if (it.contains("light"))
      light = lights_[it.at("light")];

    if (type == "sphere") {
      add_sphere(parse_point3(v.cbegin()), v.at(3), mat, light);
    } else if (type == "plane") {
      add_2d<Plane>(parse_point3(v.cbegin()), parse_point3(v.cbegin() + 3),
                    parse_point3(v.cbegin() + 6), mat, light);
    } else if (type == "triangle") {
      add_2d<Triangle>(parse_point3(v.cbegin()), parse_point3(v.cbegin() + 3),
                       parse_point3(v.cbegin() + 6), mat, light);
    } else if (type == "quad") {
      add_2d<Parallelogram>(parse_point3(v.cbegin()),
                            parse_point3(v.cbegin() + 3),
                            parse_point3(v.cbegin() + 6), mat, light);
    } else if (type == "cube") {
      add_cube(parse_point3(v.cbegin()), parse_point3(v.cbegin() + 3),
               parse_point3(v.cbegin() + 6), parse_point3(v.cbegin() + 9), mat,
               light);
    } else {
      spdlog::error("unsupported shape: {}", type);
    }
  }

  return Scene(std::move(objs_));
}

/* -- public builders ---------------------------------------------------- */

Camera SceneFactory::CreateCamera() const {
  return parse_camera(root_.at("camera"));
}

Scene SceneFactory::CreateScene() {
  Scene scene = parse_objects(root_.at("objects"));

  scene.SetBackground([](Ray) { return Color(0); });

  if (root_.contains("background")) {
    if (root_.at("background") == "sky")
      scene.SetBackground([](Ray r) -> Color {
        auto direction = r.direction.normalised();
        double a = 0.5 * (1.0 + direction.y());
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
      });
  }

  return scene;
}
