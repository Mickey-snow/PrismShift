#include "integrator.hpp"
#include "scene_factory.hpp"

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <chrono>
#include <cstdlib>  // std::exit
#include <format>

namespace chr = std::chrono;

int main(int argc, char** argv) {
  argparse::ArgumentParser program(*argv);
  program.add_argument("scene_file").required();
  program.add_argument("-o", "--output")
      .help("specify the output file.")
      .default_value("output.ppm");
  program.add_argument("--spp").scan<'d', int>().default_value(32);
  program.add_argument("--max_depth").scan<'d', int>().default_value(64);
  program.add_argument("--no_mis").default_value(false).implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << '\n' << program;
    std::exit(EXIT_FAILURE);
  }

  // build scene
  SceneFactory factory =
      SceneFactory::FromFile(program.get<std::string>("scene_file"));

  Camera camera = factory.CreateCamera();
  Scene scene = factory.CreateScene();

  spdlog::info("scene setup complete.");

  const size_t primitive_count = scene.objs_.size();
  spdlog::info("Primitive count: {}", primitive_count);

  // render
  auto begin_time = chr::high_resolution_clock::now();
  Integrator integrator(scene, program.get<int>("--max_depth"),
                        !program.get<bool>("--no_mis"));
  integrator.Render(camera, program.get<std::string>("--output"),
                    program.get<int>("--spp"));
  auto end_time = chr::high_resolution_clock::now();
  auto duration = chr::duration_cast<chr::seconds>(end_time - begin_time);
  spdlog::info("time wasted rendering: " +
               std::format("{:%H hours %M minutes %S seconds}", duration));

  const size_t rays = integrator.GetRaycount();
  spdlog::info("Primitive count: {}", primitive_count);
  spdlog::info("Ray count: {}", rays);
  spdlog::info("kRays/s: {}",
               static_cast<Float>(rays) / 1'000 / duration.count());
}
