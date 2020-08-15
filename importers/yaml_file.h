//
// Created by Brian Landers on 8/15/20.
//

#pragma once
#include <string>

#include "../core/matrix.h"
#include "../core/camera.h"
#include "../shapes/cube.h"
#include "../shapes/plane.h"
#include "file.h"
#include "yaml-cpp/yaml.h"

namespace {
Material node_to_material(const YAML::Node& n) {
  auto out = Material();
  out.set_color(Color(n["color"][0].as<double>(), n["color"][1].as<double>(),
                      n["color"][2].as<double>()));
  out.set_diffuse(n["diffuse"].as<double>(out.diffuse()));
  out.set_specular(n["specular"].as<double>(out.specular()));
  out.set_ambient(n["ambient"].as<double>(out.ambient()));
  out.set_shininess(n["shininess"].as<double>(out.shininess()));
  out.set_transparency(n["transparency"].as<double>(out.transparency()));
  out.set_reflective(n["reflective"].as<double>(out.reflective()));
  out.set_refractive(n["refractive"].as<double>(out.refractive()));
  return out;
}

Matrix node_to_transform(const YAML::Node& node) {
  Matrix out{IDENTITY};

  for (const auto& t : node) {
    auto ttype = t[0].as<std::string>();
    if (ttype == "translate") {
      std::cout << "ADD: translate" << std::endl;
      double vals[4] = { t[1].as<double>(), t[2].as<double>(),
                         t[3].as<double>() };
      auto tf = CreateTranslation(vals[0], vals[1], vals[2]);
      out = out * tf;
    } else if (ttype == "scale") {
      std::cout << "ADD: scale" << std::endl;
      double vals[4] = { t[1].as<double>(), t[2].as<double>(),
                         t[3].as<double>() };
      auto tf = CreateScaling(vals[0], vals[1], vals[2]);
      out = out * tf;
    } else {
      throw std::runtime_error(ttype);
    }
  }
  return out;
}
}

class YamlFile : public File {
 public:
  explicit YamlFile(const std::string& blob) : File(blob, false) {
    root_ = YAML::Load(blob.c_str());
    for (const auto& item : root_) {
      const auto itype = item["add"].as<std::string>();

      if (itype == "camera") {
        std::cout << "ADD: camera" << std::endl;

        auto c = new Camera(item["width"].as<int>(), item["height"].as<int>(), item["field-of-view"].as<double>());
        camera_.reset(c);

        if (auto from = item["from"]) {
          std::cout << "ADD: view_transform" << std::endl;
          auto f = Tuple::point(from[0].as<double>(), from[1].as<double>(),from[2].as<double>());

          auto to = item["to"];
          auto t = Tuple::point(to[0].as<double>(), to[1].as<double>(),to[2].as<double>());

          auto up = item["up"];
          auto u = Tuple::vector(up[0].as<double>(), up[1].as<double>(),up[2].as<double>());

          camera_->set_transform(view_transform(f, t, u));
        }
        continue;
      }

      if (itype == "point_light") {
        std::cout << "ADD: point_light" << std::endl;

        auto light_p = item["position"];
        auto light_v = item["intensity"];

        auto point = Tuple::point(light_p[0].as<double>(), light_p[1].as<double>(), light_p[2].as<double>());
        auto intensity = Color(light_v[0].as<double>(), light_v[1].as<double>(), light_v[2].as<double>());

        auto l = new PointLight(point, intensity);
        light_.reset(l);

        continue;
      }

      if (itype == "area_light") {
        std::cout << "ADD: area_light" << std::endl;

        auto corner = item["corner"];
        auto v1 = item["uvec"];
        auto v2 = item["vvec"];
        auto usteps = item["usteps"].as<size_t>();
        auto vsteps = item["vsteps"].as<size_t>();
        auto intensity = item["intensity"];
        auto corner_p = Tuple::point(corner[0].as<double>(), corner[1].as<double>(), corner[2].as<double>());
        auto v1_v = Tuple::vector(v1[0].as<double>(), v1[1].as<double>(), v1[2].as<double>());
        auto v2_v = Tuple::vector(v2[0].as<double>(), v2[1].as<double>(), v2[2].as<double>());
        auto intensity_c = Color(intensity[0].as<double>(), intensity[1].as<double>(), intensity[2].as<double>());

        auto light = new AreaLight(corner_p, v1_v, usteps, v2_v,vsteps, intensity_c);
        light_.reset(light);

        continue;
      }

      if (itype == "plane") {
        std::cout << "ADD: plane" << std::endl;
        auto s = std::make_shared<Plane>();

        if (auto t = item["transform"]) {
          auto transform = node_to_transform(t);
          s->set_transform(transform);
        }

        if (auto m = item["material"]) {
          auto mat = node_to_material(m);
          s->set_material(mat);
        }
        default_group_->add(s);
      }

      if (itype == "sphere") {
        std::cout << "ADD: sphere" << std::endl;
        auto s = std::make_shared<Sphere>();

        if (auto t = item["transform"]) {
          auto transform = node_to_transform(t);
          s->set_transform(transform);
        }

        if (auto m = item["material"]) {
          auto mat = node_to_material(m);
          s->set_material(mat);
        }
        default_group_->add(s);
      }

      if (itype == "cube") {
        std::cout << "ADD: cube" << std::endl;
        auto c = std::make_shared<Cube>();

        if (auto t = item["transform"]) {
          auto transform = node_to_transform(t);
          c->set_transform(transform);
        }

        if (auto m = item["material"]) {
          auto mat = node_to_material(m);
          c->set_material(mat);
        }
        default_group_->add(c);
      }
    }
  }

  Camera* camera() const override {
    return camera_.get();
  }

  Light* light() const override {
    return light_.get();
  }

 private:
  YAML::Node root_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Light> light_;
};
