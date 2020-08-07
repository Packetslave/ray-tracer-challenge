#include "tuple.h"

bool eq(const double a, const double b) { return abs(a - b) < EPSILON; }

double dot(const Tuple& a, const Tuple& b);

Tuple::Tuple(double x, double y, double z, double w) : x{x}, y{y}, z{z}, w{w} {}

Tuple Tuple::point(double x, double y, double z) { return Tuple(x, y, z, 1.0); }

Tuple Tuple::point(const Tuple& rhs) {
  if (rhs.w != 1.0) {
    throw new std::runtime_error("not a point");
  }
  return Tuple(rhs.x, rhs.y, rhs.z, 1.0);
}

Tuple Tuple::vector(double x, double y, double z) {
  return Tuple(x, y, z, 0.0);
}

Tuple Tuple::vector(const Tuple& rhs) {
  if (rhs.w != 0.0) {
    throw new std::runtime_error("not a vector");
  }
  return Tuple(rhs.x, rhs.y, rhs.z, 0.0);
}

bool Tuple::point() const { return w == 1.0; }

bool Tuple::vector() const { return w == 0.0; }

double Tuple::magnitude() const { return sqrt(x * x + y * y + z * z + w * w); }

Tuple Tuple::normalize() const {
  const double mag = magnitude();
  return Tuple(x / mag, y / mag, z / mag, w / mag);
}

Tuple Tuple::reflect(const Tuple& n) { return *this - n * 2 * dot(*this, n); }

std::ostream& operator<<(std::ostream& os, const Tuple& rhs) {
  return os << "Tuple(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ", "
            << rhs.w << ")";
}

Tuple& Tuple::operator+=(const Tuple& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;

  return *this;
}

Tuple& Tuple::operator-=(const Tuple& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;

  return *this;
}

Tuple& Tuple::operator*=(const double rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;

  return *this;
}

Tuple& Tuple::operator/=(const double rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;

  return *this;
}

Tuple operator+(Tuple lhs, const Tuple& rhs) {
  lhs += rhs;
  return lhs;
}

Tuple operator-(Tuple lhs, const Tuple& rhs) {
  lhs -= rhs;
  return lhs;
}

Tuple operator*(Tuple lhs, const double rhs) {
  lhs *= rhs;
  return lhs;
}

Tuple operator/(Tuple lhs, const double rhs) {
  lhs /= rhs;
  return lhs;
}

Tuple operator-(const Tuple& rhs) {
  return Tuple(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

bool operator==(Tuple const& lhs, Tuple const& rhs) {
  return eq(lhs.x, rhs.x) && eq(lhs.y, rhs.y) && eq(lhs.z, rhs.z) &&
         eq(lhs.w, rhs.w);
}

double dot(const Tuple& a, const Tuple& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Tuple cross(const Tuple& a, const Tuple& b) {
  return Tuple::vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
}
