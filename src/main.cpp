#include "integrator.hpp"
#include "scene_factory.hpp"

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <chrono>
#include <cstdlib>  // std::exit
#include <format>

namespace chr = std::chrono;

int main(int argc, char** argv) {
  auto begin_time = chr::high_resolution_clock::now();

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

  // render
  Integrator integrator(scene, program.get<int>("--max_depth"));
  integrator.Render(camera, program.get<std::string>("--output"),
                    program.get<int>("--spp"),
                    !program.get<bool>("--no_mis"));

  auto end_time = chr::high_resolution_clock::now();
  auto duration = chr::duration_cast<chr::seconds>(end_time - begin_time);
  spdlog::info("time wasted rendering: " +
               std::format("{:%H hours %M minutes %S seconds}", duration));
}
