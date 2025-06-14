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

inline static std::shared_ptr<Primitive> make_sphere(size_t mat_id,
                                                     Point3 pos,
                                                     double d) {
  static const auto sphere = std::make_shared<Sphere>();

  std::shared_ptr<Material> mat = materials[mat_id];
  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  return std::make_shared<Primitive>(sphere, std::move(mat), std::move(trans));
}

Scene parse_scene(json& root) {
  std::vector<std::shared_ptr<Primitive>> objs;

  for (auto& it : root) {
    if (it["type"] != "sphere")
      spdlog::error("unsopported shape: {}", it["type"].get<std::string>());

    auto& v = it["data"];
    objs.emplace_back(
        make_sphere(v.at(0), Point3(v.at(1), v.at(2), v.at(3)), v.at(4)));
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
    spdlog::info("loading scene from file: {} ...", scnfile);
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
