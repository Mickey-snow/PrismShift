#pragma once

#include "camera.hpp"
#include "primitive.hpp"
#include "scene.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

class Parallelogram;
class Sphere;
class Triangle;
class ILight;

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
  [[nodiscard]] Scene CreateScene();

 private:
  /* ---------- parsing helpers ---------- */
  static Camera parse_camera(const nlohmann::json& r);
  Scene parse_objects(const nlohmann::json& array);
  void parse_materials(const nlohmann::json& array);
  void parse_lights(const nlohmann::json& r);

 private:
  nlohmann::json root_;  // raw JSON tree

  std::vector<std::shared_ptr<IMaterial>> materials_;
  std::unordered_map<std::string, std::shared_ptr<IMaterial>> material_map_;

  std::vector<std::shared_ptr<ILight>> lights_;
  std::unordered_map<std::string, std::shared_ptr<ILight>> light_map_;

  std::vector<std::shared_ptr<Primitive>> objs_;
};
