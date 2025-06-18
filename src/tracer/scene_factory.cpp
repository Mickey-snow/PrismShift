#include "scene_factory.hpp"

#include "light.hpp"
#include "material.hpp"
#include "shapes/shape.hpp"
#include "texture.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

using nlohmann::json;

namespace {

[[nodiscard]] std::runtime_error make_error(std::string_view ctx,
                                            const std::string& detail,
                                            const char* file,
                                            int line) {
  std::ostringstream oss;
  oss << file << ':' << line << " – " << ctx << ": " << detail;
  return std::runtime_error(oss.str());
}

#define SCENE_ERROR(ctx, detail) make_error(ctx, detail, __FILE__, __LINE__)

template <typename T>
T expect_number(const json& j, std::string_view ctx) {
  if (!j.is_number())
    throw SCENE_ERROR(ctx, "expected a number but got " + j.dump());
  return j.get<T>();
}

void expect_array_size(const json& j,
                       std::size_t at_least,
                       std::string_view ctx) {
  if (!j.is_array())
    throw SCENE_ERROR(ctx, "expected an array but got " + j.dump());
  if (j.size() < at_least) {
    std::ostringstream oss;
    oss << "needs at least " << at_least << " elements, but has " << j.size();
    throw SCENE_ERROR(ctx, oss.str());
  }
}

inline Point3 parse_point3(const json& arr,
                           std::size_t offset,
                           std::string_view ctx) {
  expect_array_size(arr, offset + 3, ctx);
  return {expect_number<double>(arr[offset + 0], ctx),
          expect_number<double>(arr[offset + 1], ctx),
          expect_number<double>(arr[offset + 2], ctx)};
}

}  // namespace

Camera SceneFactory::parse_camera(const json& r) {
  constexpr std::string_view ctx = "camera";
  expect_array_size(r, 9, ctx);

  Point3 center = parse_point3(r, 0, ctx);
  Point3 look_at = parse_point3(r, 3, ctx);
  int image_h = expect_number<int>(r[6], "camera.image_h");
  double aspect = expect_number<double>(r[7], "camera.aspect");
  double vFovDeg = expect_number<double>(r[8], "camera.vFovDeg");

  return Camera(center, look_at, image_h, aspect, vFovDeg);
}

// ------------------------------------------------------------------------------
SceneFactory::SceneFactory(json root) : root_(std::move(root)) {
  if (root_.contains("materials"))
    parse_materials(root_.at("materials"));
  if (root_.contains("lights"))
    parse_lights(root_.at("lights"));
}

SceneFactory SceneFactory::FromFile(const std::string& path) {
  std::ifstream ifs(path);
  if (!ifs)
    throw SCENE_ERROR("file-load", "cannot open scene file: " + path);

  spdlog::info("Loading scene from file '{}'", path);
  json root;
  try {
    ifs >> root;
  } catch (const std::exception& e) {
    throw SCENE_ERROR("file-load",
                      std::string("JSON parse error: ") + e.what());
  }

  return SceneFactory(std::move(root));
}

// ------------------------------------------------------------------------------
void SceneFactory::parse_materials(const json& array) {
  constexpr std::string_view ctx = "materials";
  expect_array_size(array, 1, ctx);
  materials_.reserve(array.size());

  for (std::size_t idx = 0; idx < array.size(); ++idx) {
    const json& it = array[idx];
    const auto& type = it.at("type");

    if (!type.is_string())
      throw SCENE_ERROR(
          ctx, "`type` must be string at material #" + std::to_string(idx));

    const std::string type_s = type.get<std::string>();
    const json& v = it.at("data");
    expect_array_size(v, 1, ctx);

    std::shared_ptr<IMaterial> mat;

    if (type_s == "conductor") {
      expect_array_size(v, 4, "conductor data");
      Color col{expect_number<double>(v[0], "conductor.color[0]"),
                expect_number<double>(v[1], "conductor.color[1]"),
                expect_number<double>(v[2], "conductor.color[2]")};
      double uR = expect_number<double>(v[3], "conductor.uRough");
      double vR = (v.size() >= 5)
                      ? expect_number<double>(v[4], "conductor.vRough")
                      : uR;
      mat = std::make_shared<ConductorMaterial>(col, uR, vR);

    } else if (type_s == "diffuse") {
      expect_array_size(v, 3, "diffuse data");
      Color col{expect_number<double>(v[0], "diffuse.color[0]"),
                expect_number<double>(v[1], "diffuse.color[1]"),
                expect_number<double>(v[2], "diffuse.color[2]")};
      mat = std::make_shared<DiffuseMaterial>(col);

    } else if (type_s == "dielectric") {
      expect_array_size(v, 2, "dielectric data");
      double eta = expect_number<double>(v[0], "dielectric.eta");
      double uR = expect_number<double>(v[1], "dielectric.uRough");
      double vR = (v.size() >= 3)
                      ? expect_number<double>(v[2], "dielectric.vRough")
                      : uR;
      mat = std::make_shared<DielectricMaterial>(eta, uR, vR);

    } else if (type_s == "mix") {
      expect_array_size(v, 2, "mixed material data");
      std::shared_ptr<IMaterial> mat1 = resolve_mat(v[0]),
                                 mat2 = resolve_mat(v[1]);
      double fac = expect_number<double>(v[2], "mix.fac");
      mat = std::make_shared<MixedMaterial>(mat1, mat2, fac);
    } else {
      throw SCENE_ERROR(ctx, "unknown material type '" + type_s +
                                 "' at index " + std::to_string(idx));
    }

    /* optional name -> map */
    if (it.contains("name") && it["name"].is_string())
      material_map_[it["name"].get<std::string>()] = mat;

    materials_.emplace_back(std::move(mat));
  }
}

// ------------------------------------------------------------------------------
void SceneFactory::parse_lights(const json& array) {
  constexpr std::string_view ctx = "lights";
  expect_array_size(array, 1, ctx);
  lights_.reserve(array.size());

  for (std::size_t idx = 0; idx < array.size(); ++idx) {
    const json& it = array[idx];
    const auto& type = it.at("type");

    if (!type.is_string())
      throw SCENE_ERROR(
          ctx, "`type` must be string at light #" + std::to_string(idx));
    if (type != "diffuse_light")
      throw SCENE_ERROR(
          ctx, "unsupported light type '" + type.get<std::string>() + "'");

    const json& v = it.at("data");
    expect_array_size(v, 3, "light data");

    auto light = std::make_shared<Light>(
        Color(expect_number<double>(v[0], "light.color[0]"),
              expect_number<double>(v[1], "light.color[1]"),
              expect_number<double>(v[2], "light.color[2]")));

    if (it.contains("name") && it["name"].is_string())
      light_map_[it["name"].get<std::string>()] = light;

    lights_.emplace_back(std::move(light));
  }
}

// ------------------------------------------------------------------------------
Scene SceneFactory::parse_objects(const json& array) {
  constexpr std::string_view ctx = "objects";
  expect_array_size(array, 1, ctx);

  objs_.clear();
  objs_.reserve(array.size());

  for (std::size_t idx = 0; idx < array.size(); ++idx) {
    const json& it = array[idx];
    const auto& type = it.at("type");
    if (!type.is_string())
      throw SCENE_ERROR(
          ctx, "`type` must be string at object #" + std::to_string(idx));
    const std::string type_s = type.get<std::string>();

    const json& v = it.at("data");
    expect_array_size(v, 1, "object.data");

    /* -------- material / light lookup -------------------------------- */
    std::shared_ptr<IMaterial> mat = nullptr;
    std::shared_ptr<ILight> light = nullptr;

    if (it.contains("material"))
      mat = resolve_mat(it["material"]);
    if (it.contains("light"))
      light = resolve_light(it["light"]);

    // shape construction
    if (type_s == "sphere") {
      expect_array_size(v, 4, "sphere data");
      add_object(
          MakePrimitive<Sphere>(mat, light, parse_point3(v, 0, "sphere.center"),
                                expect_number<double>(v[3], "sphere.radius")));

    } else if (type_s == "plane") {
      expect_array_size(v, 9, "plane data");
      add_object(MakePrimitive<Plane>(
          mat, light, parse_point3(v, 0, "plane.p0"),
          parse_point3(v, 3, "plane.p1"), parse_point3(v, 6, "plane.p2")));

    } else if (type_s == "triangle") {
      expect_array_size(v, 9, "triangle data");
      add_object(MakePrimitive<Triangle>(mat, light,
                                         parse_point3(v, 0, "triangle.p0"),
                                         parse_point3(v, 3, "triangle.p1"),
                                         parse_point3(v, 6, "triangle.p2")));

    } else if (type_s == "quad") {
      expect_array_size(v, 9, "quad data");
      add_object(MakePrimitive<Parallelogram>(
          mat, light, parse_point3(v, 0, "quad.p0"),
          parse_point3(v, 3, "quad.p1"), parse_point3(v, 6, "quad.p2")));

    } else if (type_s == "cube") {
      expect_array_size(v, 12, "cube data");
      Point3 o = parse_point3(v, 0, "cube.origin");
      Point3 a = parse_point3(v, 3, "cube.a");
      Point3 b = parse_point3(v, 6, "cube.b");
      Point3 c = parse_point3(v, 9, "cube.c");

      Vector3 e1 = a - o, e2 = b - o, e3 = c - o;
      add_object(MakePrimitive<Parallelogram>(mat, light, o, a, b));
      add_object(MakePrimitive<Parallelogram>(mat, light, o, a, c));
      add_object(MakePrimitive<Parallelogram>(mat, light, o, b, c));

      o += e1 + e2 + e3;  // opposite corner
      a = o - e1;
      b = o - e2;
      c = o - e3;
      add_object(MakePrimitive<Parallelogram>(mat, light, o, a, b));
      add_object(MakePrimitive<Parallelogram>(mat, light, o, a, c));
      add_object(MakePrimitive<Parallelogram>(mat, light, o, b, c));

    } else {
      throw SCENE_ERROR(ctx, "unsupported shape '" + type_s + "' at object #" +
                                 std::to_string(idx));
    }
  }

  return Scene(std::move(objs_));
}

std::shared_ptr<IMaterial> SceneFactory::resolve_mat(const json& m) {
  if (m.is_number_unsigned()) {
    if (m.get<std::size_t>() >= materials_.size())
      throw std::runtime_error("material index out of range.");
    return materials_[m];
  }
  if (m.is_string()) {
    const auto& key = m.get<std::string>();
    if (!material_map_.contains(key))
      throw std::runtime_error("material '" + key + "' not found.");
    return material_map_.at(key);
  }
  throw std::runtime_error("invalid material reference.");
}
std::shared_ptr<ILight> SceneFactory::resolve_light(const json& l) {
  if (l.is_number_unsigned()) {
    if (l.get<std::size_t>() >= lights_.size())
      throw std::runtime_error("light index out of range.");
    return lights_[l];
  }
  if (l.is_string()) {
    const auto& key = l.get<std::string>();
    if (!light_map_.contains(key))
      throw std::runtime_error("light '" + key + "' not found");
    return light_map_.at(key);
  }
  throw std::runtime_error("invalid light reference.");
}

// ------------------------------------------------------------------------------
Camera SceneFactory::CreateCamera() const {
  if (!root_.contains("camera"))
    throw SCENE_ERROR("factory", "scene JSON missing top-level 'camera' field");
  return parse_camera(root_.at("camera"));
}

Scene SceneFactory::CreateScene() {
  if (!root_.contains("objects"))
    throw SCENE_ERROR("factory",
                      "scene JSON missing top-level 'objects' field");

  Scene scene = parse_objects(root_.at("objects"));
  scene.SetBackground([](Ray) { return Color(0); });

  if (root_.contains("background") && root_["background"] == "sky") {
    scene.SetBackground([](Ray r) -> Color {
      auto dir = r.direction.normalised();
      double a = 0.5 * (1.0 + dir.y());
      return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    });
  }
  return scene;
}
