#pragma once

#include <array>
#include <cmath>
#include <ostream>

static constexpr float EPSILON = 0.0001;

bool eq(const float a, const float b) { return abs(a - b) < EPSILON; }

class Tuple;
float dot(const Tuple& a, const Tuple& b);

class Tuple {
 public:
  Tuple(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w} {}

  static Tuple point(float x, float y, float z) { return Tuple(x, y, z, 1.0); }

  static Tuple point(const Tuple& rhs) {
    if (rhs.w != 1.0) {
      throw new std::runtime_error("not a point");
    }
    return Tuple(rhs.x, rhs.y, rhs.z, 1.0);
  }

  static Tuple vector(float x, float y, float z) { return Tuple(x, y, z, 0.0); }

  static Tuple vector(const Tuple& rhs) {
    if (rhs.w != 0.0) {
      throw new std::runtime_error("not a vector");
    }
    return Tuple(rhs.x, rhs.y, rhs.z, 0.0);
  }

  bool point() const { return w == 1.0; }

  bool vector() const { return w == 0.0; }

  float magnitude() const { return sqrt(x * x + y * y + z * z + w * w); }

  Tuple normalize() const {
    const float mag = magnitude();
    return Tuple(x / mag, y / mag, z / mag, w / mag);
  }

  Tuple reflect(const Tuple& n) { return *this - n * 2 * dot(*this, n); }

  friend std::ostream& operator<<(std::ostream& os, const Tuple& rhs) {
    return os << "Tuple(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ", "
              << rhs.w << ")";
  }

  Tuple& operator+=(const Tuple& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;

    return *this;
  }

  Tuple& operator-=(const Tuple& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;

    return *this;
  }

  Tuple& operator*=(const float rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;

    return *this;
  }

  Tuple& operator/=(const float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;

    return *this;
  }

  friend Tuple operator+(Tuple lhs, const Tuple& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Tuple operator-(Tuple lhs, const Tuple& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend Tuple operator*(Tuple lhs, const float rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend Tuple operator/(Tuple lhs, const float rhs) {
    lhs /= rhs;
    return lhs;
  }

  float x, y, z, w;
};

Tuple operator-(const Tuple& rhs) {
  return Tuple(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

inline bool operator==(const Tuple& lhs, const Tuple& rhs) {
  return eq(lhs.x, rhs.x) && eq(lhs.y, rhs.y) && eq(lhs.z, rhs.z) &&
         eq(lhs.w, rhs.w);
}

float dot(const Tuple& a, const Tuple& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Tuple cross(const Tuple& a, const Tuple& b) {
  return Tuple::vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
}
