#pragma once
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#pragma once

#include "camera.hpp"
#include "primitive.hpp"
#include "scene.hpp"

/**
 * SceneFactory
 * ------------
 *  ‣ Encapsulates all details of reading a JSON scene file
 *    and turning it into Camera / Scene objects.
 */
class SceneFactory {
 public:
  /** Load from an already-parsed JSON object. */
  explicit SceneFactory(nlohmann::json root);

  /** Convenience helper: load JSON from disk, then build a factory. */
  static SceneFactory FromFile(const std::string& path);

  /** Build camera described in the JSON.  */
  [[nodiscard]] Camera CreateCamera() const;

  /** Build full scene (materials, primitives, etc.). */
  [[nodiscard]] Scene CreateScene() const;

 private:
  /* ---------- parsing helpers ---------- */
  static Camera parse_camera(const nlohmann::json& r);

  void parse_materials(const nlohmann::json& array);

  std::shared_ptr<Primitive> make_sphere(std::size_t matId,
                                         Point3 pos,
                                         double d) const;

  template <class T>
    requires std::is_base_of_v<IShape, T>
  std::shared_ptr<Primitive> make_2d(std::size_t matId,
                                     Point3 o,
                                     Point3 a,
                                     Point3 b) const {
    static const std::shared_ptr<IShape> shape = std::make_shared<T>();

    std::shared_ptr<Material> mat = nullptr;
    if (matId < materials_.size())
      mat = materials_[matId];
    else
      spdlog::error("material id ({}) out of bounds.", matId);

    auto trans = std::make_shared<MatrixTransformation>(
        MatrixTransformation::TriangleToUnit(std::move(o), std::move(a),
                                             std::move(b)));
    return std::make_shared<Primitive>(shape, std::move(mat), std::move(trans));
  }

  Scene parse_scene(const nlohmann::json& array) const;

private:
  nlohmann::json root_;  // raw JSON tree
  std::vector<std::shared_ptr<Material>> materials_;
};
