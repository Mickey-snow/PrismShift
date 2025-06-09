#include "imp.hpp"

int main() {
  auto camera = Get_Camera();
  auto scene = Get_World();
  render_ppm(scene, camera, "output.ppm");
}
