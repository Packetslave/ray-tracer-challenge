#include "light.h"
#include "world.h"

double PointLight::intensity_at(const Tuple& point, const World* world) const {
  return !(world->is_shadowed(position_, point));
}

double AreaLight::intensity_at(const Tuple& point, const World* world) const {
  double total = 0.0;

  for (size_t v = 0; v < vsteps_; ++v) {
    for (size_t u = 0; u < usteps_; ++u) {
      auto light_position = point_on(u, v);
      if (!(world->is_shadowed(light_position, point))) {
        total += 1.0;
      }
    }
  }
  return total / sampleCount_;
}
