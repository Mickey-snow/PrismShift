#include "imp.hpp"

int main() {
  auto camera = Get_Camera();
  auto scene = Get_World();

  int image_width = 400;
  double aspect_ratio = 16.0 / 9.0;
  int image_height = static_cast<int>(image_width / aspect_ratio);

  render_ppm(scene, camera, image_width, image_height, "output.ppm");
  return 0;
}
