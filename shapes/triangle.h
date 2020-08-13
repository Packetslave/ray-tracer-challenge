//
// Created by Brian Landers on 8/8/20.
//

#include "../core/bounding_box.h"
#include "shape.h"

#pragma once

class Triangle : public Shape {
 public:
  Triangle(const Tuple& p1, const Tuple& p2, const Tuple& p3)
      : Shape(),
        p1{p1},
        p2{p2},
        p3{p3},
        e1{p2 - p1},
        e2{p3 - p1},
        normal{cross(e2, e1).normalize()} {
    box_.add(p1);
    box_.add(p2);
    box_.add(p3);
  }

  bool compare(const Shape&) const noexcept override { return true; }

  int max_axis(double* ray) {
    auto x = abs(ray[0]);
    auto y = abs(ray[1]);
    auto z = abs(ray[2]);

    if (x > y && x > z) {
      return 0;
    }
    if (y > z && y > z) {
      return 1;
    }
    return 2;
  }

  IntersectionVector local_intersect(const Ray& r) override {
    double ray[3] = { r.direction().x, r.direction().y, r.direction().z };
    auto origin = r.origin();

    int kz = max_axis(ray);
    int kx = kz + 1;
    if (kx == 0) { kx = 0; }
    int ky = kx + 1;
    if (ky == 0) { ky = 0; }

    if (ray[kz] < 0.0f) {
      int swap = ky;
      ky = kx;
      kx = swap;
    }

    //if (ray[kz] == 0) {
      // fix
    //}

    double Sx = ray[kx] / ray[kz];
    double Sy = ray[ky] / ray[kz];
    double Sz = 1.0f / ray[kz];

    auto A = p1 - origin;
    double Ar[] = { A.x, A.y, A.z };

    auto B = p2 - origin;
    double Br[] = { B.x, B.y, B.z };

    auto C = p3 - origin;
    double Cr[] = { C.x, C.y, C.x };

    auto Ax = Ar[kx] - Sx * Ar[kz];
    auto Ay = Ar[ky] - Sy * Ar[kz];
    auto Bx = Br[kx] - Sx * Br[kz];
    auto By = Br[ky] - Sy * Br[kz];
    auto Cx = Cr[kx] - Sx * Cr[kz];
    auto Cy = Cr[ky] - Sy * Cr[kz];

    auto U = Cx * By - Cy * Bx;
    auto V = Ax * Cy - Ay * Cx;
    auto W = Bx * Ay - By * Ax;

    if (U == 0.0f || V == 0.0f || W == 0.0f) {
      double CxBy = (double)Cx*(double)By;
      double CyBx = (double)Cy*(double)Bx;
      U = (float)(CxBy - CyBx);
      double AxCy = (double)Ax*(double)Cy;
      double AyCx = (double)Ay*(double)Cx;
      V = (float)(AxCy - AyCx);
      double BxAy = (double)Bx*(double)Ay;
      double ByAx = (double)By*(double)Ax;
      W = (float)(BxAy - ByAx);
    }

    if ((U<0.0f || V<0.0f || W<0.0f) &&
        (U>0.0f || V>0.0f || W>0.0f)) return {};

    float det = U+V+W;
    if (det == 0.0f) { return {}; }

    const float Az = Sz*Ar[kz];
    const float Bz = Sz*Br[kz];
    const float Cz = Sz*Cr[kz];
    const float T = U*Az + V*Bz + W*Cz;

    auto xorf_T = std::abs(T);
    if(std::signbit(T) != std::signbit(det))
      xorf_T = -xorf_T;

    auto ray_near = 0.0f;
    auto hit_t = std::numeric_limits<float_t>::infinity();
    auto abs_det = std::abs(det);
    if(xorf_T < ray_near * abs_det || hit_t * abs_det < xorf_T)
      return {};

    const float rcpDet = 1.0f/det;
    auto out_u = U*rcpDet;
    auto out_v = V*rcpDet;
    auto out_w = W*rcpDet;
    auto out_t = T*rcpDet;

    return {
        Intersection(out_t, this)
    };

//    auto dir_cross_e2 = cross(r.direction(), e2);
//    auto det = dot(e1, dir_cross_e2);
//    if (abs(det) < EPSILON) {
//      return {};
//    }
//    auto f = 1.0 / det;
//    auto p1_to_origin = r.origin() - p1;
//    auto u = f * dot(p1_to_origin, dir_cross_e2);
//    if (u < 0 || u > 1) {
//      return {};
//    }
//
//    auto origin_cross_e1 = cross(p1_to_origin, e1);
//    auto v = f * dot(r.direction(), origin_cross_e1);
//    if (v < 0 || (u + v) > 1) {
//      return {};
//    }
//
//    auto t = f * dot(e2, origin_cross_e1);
//    return {Intersection(t, this)};
  };

  Tuple local_normal_at(const Tuple& p, const Intersection* i) override {
    return normal;
  }

  BoundingBox* bounds_of() override { return &box_; }

  Tuple p1, p2, p3;
  Tuple e1, e2, normal;
  BoundingBox box_;
};

class SmoothTriangle : public Triangle {
 public:
  SmoothTriangle(const Tuple& p1, const Tuple& p2, const Tuple& p3,
                 const Tuple& n1, const Tuple& n2, const Tuple& n3)
      : Triangle(p1, p2, p3), n1{n1}, n2{n2}, n3{n3} {}

  Tuple local_normal_at(const Tuple& p, const Intersection* i) override {
    return n2 * i->u + n3 * i->v + n1 * (1 - i->u - i->v);
  }

  IntersectionVector local_intersect(const Ray& r) override {
    auto dir_cross_e2 = cross(r.direction(), e2);
    auto det = dot(e1, dir_cross_e2);
    if (abs(det) < EPSILON) {
      return {};
    }
    auto f = 1.0 / det;
    auto p1_to_origin = r.origin() - p1;
    auto u = f * dot(p1_to_origin, dir_cross_e2);
    if (u < 0 || u > 1) {
      return {};
    }

    auto origin_cross_e1 = cross(p1_to_origin, e1);
    auto v = f * dot(r.direction(), origin_cross_e1);
    if (v < 0 || (u + v) > 1) {
      return {};
    }

    auto t = f * dot(e2, origin_cross_e1);
    return {Intersection(t, this, u, v)};
  }

  Tuple n1, n2, n3;
};