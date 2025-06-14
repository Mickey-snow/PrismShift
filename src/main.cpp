#include "integrator.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "shapes/shape.hpp"
#include "texture.hpp"
#include "util/util.hpp"

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using nlohmann::json;

std::vector<std::shared_ptr<Material>> materials;

Camera parse_camera(json& r) {
  Point3 center(r.at(0), r.at(1), r.at(2));
  Point3 lookat(r.at(3), r.at(4), r.at(5));
  int image_h = r.at(6);
  double aspect = r.at(7);
  double vFovDeg = r.at(8);
  return Camera(center, lookat, image_h, aspect, vFovDeg);
}

void parse_materials(json& root) {
  for (auto& it : root) {
    std::string type = it["type"];
    auto& v = it["data"];

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

    materials.emplace_back(std::move(mat));
  }
}

static std::shared_ptr<Primitive> make_sphere(size_t mat_id,
                                              Point3 pos,
                                              double d) {
  static const auto sphere = std::make_shared<Sphere>();

  std::shared_ptr<Material> mat = nullptr;
  if (mat_id < materials.size())
    mat = materials[mat_id];
  else
    spdlog::error("material id ({}) out of bounds.", mat_id);

  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  return std::make_shared<Primitive>(sphere, std::move(mat), std::move(trans));
}

template <class T>
  requires std::is_base_of_v<IShape, T>
static std::shared_ptr<Primitive> make_2d(size_t mat_id,
                                          Point3 o,
                                          Point3 a,
                                          Point3 b) {
  static const std::shared_ptr<IShape> shape = std::make_shared<T>();

  std::shared_ptr<Material> mat = nullptr;
  if (mat_id < materials.size())
    mat = materials[mat_id];
  else
    spdlog::error("material id ({}) out of bounds.", mat_id);

  auto trans = std::make_shared<MatrixTransformation>(
      MatrixTransformation::TriangleToUnit(std::move(o), std::move(a),
                                           std::move(b)));
  return std::make_shared<Primitive>(shape, std::move(mat), std::move(trans));
}

Scene parse_scene(json& root) {
  std::vector<std::shared_ptr<Primitive>> objs;

  for (auto& it : root) {
    const std::string type = it["type"];
    auto& v = it["data"];

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

    } else
      spdlog::error("unsopported shape: {}", type);
  }
  return Scene(std::move(objs));
}

int main(int argc, char** argv) {
  argparse::ArgumentParser program(*argv);
  program.add_argument("scene_file").required();
  program.add_argument("-o", "--output")
      .help("specify the output file.")
      .default_value("output.ppm");
  program.add_argument("--spp").default_value(32);
  program.add_argument("--max_depth").default_value(64);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  json root;
  {
    std::string scnfile = program.get<std::string>("scene_file");
    spdlog::info("loading scene from file: '{}' ...", scnfile);
    std::ifstream ifs(scnfile);
    ifs >> root;
  }

  auto camera = parse_camera(root["camera"]);
  parse_materials(root["materials"]);
  auto scene = parse_scene(root["objects"]);
  scene.SetBackground([](Ray r) -> Color {
    auto direction = r.direction.normalised();
    double a = 0.5 * (1.0 + direction.y());
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  });

  spdlog::info("scene setup complete.");

  Integrator integrator(scene, program.get<int>("--max_depth"));
  integrator.Render(camera, program.get<std::string>("--output"),
                    program.get<int>("--spp"));
}
