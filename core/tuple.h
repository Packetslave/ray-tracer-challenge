#pragma once

#include <array>
#include <cfloat>
#include <cmath>
#include <ostream>

static constexpr double EPSILON = 0.0001;

constexpr double PI = 3.14159265358979323846;
constexpr double PI_2 = 1.57079632679489661923;
constexpr double PI_3 = 1.04719755119659774615;
constexpr double PI_4 = 0.785398163397448309616;
constexpr double PI_5 = 0.6283185307179586477;
constexpr double SQRT2_2 = 0.70710678118654752441;

bool eq(const double a, const double b);

class Tuple;
double dot(const Tuple& a, const Tuple& b);

class Tuple {
 public:
  Tuple(double x, double y, double z, double w);

  static Tuple point(double x, double y, double z);

  static Tuple point(const Tuple& rhs);

  static Tuple vector(double x, double y, double z);

  static Tuple vector(const Tuple& rhs);

  bool point() const;

  bool vector() const;

  double magnitude() const;

  Tuple normalize() const;

  Tuple reflect(const Tuple& n);

  friend std::ostream& operator<<(std::ostream& os, const Tuple& rhs);

  Tuple& operator+=(const Tuple& rhs);

  Tuple& operator-=(const Tuple& rhs);

  Tuple& operator*=(const double rhs);

  Tuple& operator/=(const double rhs);

  friend Tuple operator+(Tuple lhs, const Tuple& rhs);

  friend Tuple operator-(Tuple lhs, const Tuple& rhs);

  friend Tuple operator*(Tuple lhs, const double rhs);

  friend Tuple operator/(Tuple lhs, const double rhs);

  double x, y, z, w;
};

Tuple operator-(const Tuple& rhs);

bool operator==(Tuple const& lhs, Tuple const& rhs);

inline double dot(const Tuple& a, const Tuple& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline Tuple cross(const Tuple& a, const Tuple& b) {
  return Tuple::vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
}

const static Tuple POSITIVE_INF(DBL_MAX, DBL_MAX, DBL_MAX, 1);
const static Tuple NEGATIVE_INF(-DBL_MAX, -DBL_MAX, -DBL_MAX, 1);
