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

/**
 * SceneFactory
 * ------------
 *  - Encapsulates all details of reading a JSON scene file
 *    and turning it into Camera / Scene objects.
 *  - Throws std::runtime_error on any malformed input;
 */
class SceneFactory {
 public:
  using json = nlohmann::json;

  /** Load from an already-parsed JSON object. */
  explicit SceneFactory(json root);

  /** Convenience helper: load JSON from disk, then build a factory. */
  static SceneFactory FromFile(const std::string& path);

  /** Build camera described in the JSON.  */
  [[nodiscard]] Camera CreateCamera() const;

  /** Build full scene. */
  [[nodiscard]] Scene CreateScene();

 private:
  inline void add_object(std::shared_ptr<Primitive> o) {
    if (o != nullptr)
      objs_.emplace_back(o);
  }
  static Camera parse_camera(const json& r);
  Scene parse_objects(const json& array);
  void parse_materials(const json& array);
  void parse_lights(const json& array);
  std::shared_ptr<IMaterial> resolve_mat(const json& m);
  std::shared_ptr<ILight> resolve_light(const json& l);

 private:
  json root_;  // raw JSON tree

  std::vector<std::shared_ptr<IMaterial>> materials_;
  std::unordered_map<std::string, std::shared_ptr<IMaterial>> material_map_;

  std::vector<std::shared_ptr<ILight>> lights_;
  std::unordered_map<std::string, std::shared_ptr<ILight>> light_map_;

  std::vector<std::shared_ptr<Primitive>> objs_;
};
