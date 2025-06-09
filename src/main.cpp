#include "util/geometry.hpp"
#include "util/transform.hpp"

signed main(void) {
  Vector3 v{1, -2, 3.5};
  Vector3 fr{1, 0, 0}, to{0, 1, 0};
  auto tr = MatrixTransformation::RotateFrTo(fr, to);

  auto vp = tr.Doit(v);

  std::cout << vp << std::endl;

  return 0;
}
