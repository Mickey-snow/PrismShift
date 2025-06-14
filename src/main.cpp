#include "integrator.hpp"
#include "scene_factory.hpp"

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <cstdlib>  // std::exit

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
    std::cerr << err.what() << '\n' << program;
    std::exit(EXIT_FAILURE);
  }

  /* ------------ build camera & scene through factory ---------------- */
  SceneFactory factory =
      SceneFactory::FromFile(program.get<std::string>("scene_file"));

  Camera camera = factory.CreateCamera();
  Scene scene = factory.CreateScene();


  spdlog::info("scene setup complete.");

  /* --------------------------- render ------------------------------ */
  Integrator integrator(scene, program.get<int>("--max_depth"));
  integrator.Render(camera, program.get<std::string>("--output"),
                    program.get<int>("--spp"));
}
