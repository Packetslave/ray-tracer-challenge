#include "gtest/gtest.h"

#include "../cube.h"
#include "../ray.h"
#include "../shape.h"

TEST(Cube, RayIntersects) {
  std::shared_ptr<Shape> s;
  s.reset(new Cube());

  std::vector<std::tuple<Tuple, Tuple, double, double>> tests{
      { Tuple::point(5, 0.5, 0), Tuple::vector(-1, 0, 0), 4, 6 },
      { Tuple::point(-5, 0.5, 0), Tuple::vector(1, 0, 0), 4, 6 },
      { Tuple::point(0.5, 5, 0), Tuple::vector(0, -1, 0), 4, 6 },
      { Tuple::point(0.5, -5, 0), Tuple::vector(0, 1, 0), 4, 6 },
      { Tuple::point(0.5, 0, 5), Tuple::vector(0, 0, -1), 4, 6 },
      { Tuple::point(0.5, 0, -5), Tuple::vector(0, 0, 1), 4, 6 },
      { Tuple::point(0, 0.5, 0), Tuple::vector(0, 0, 1), -1, 1 },

  };

  for (const auto& [origin, direction, t1, t2] : tests) {
    auto r = Ray(origin, direction);
    auto xs = s->local_intersect(r);
    EXPECT_EQ(2, xs.size());
    EXPECT_EQ(t1, xs[0].t());
    EXPECT_EQ(t2, xs[1].t());
  }
}

TEST(Cube, RayMisses) {
  std::shared_ptr<Shape> s;
  s.reset(new Cube());

  std::vector<std::tuple<Tuple, Tuple>> tests{
      {Tuple::point(-2, 0, 0),   Tuple::vector(0.2673, 0.5345, 0.8018)},
      {Tuple::point(0, -2, 0),   Tuple::vector(0.8018, 0.2673, 0.5345) },
      {Tuple::point(0, 0, -2),   Tuple::vector(0.5345, 0.8018, 0.2673) },
      {Tuple::point(2, 0, 2),    Tuple::vector(0, 0, -1)               },
      {Tuple::point(0, 2, 2),    Tuple::vector(0, -1, 0)               },
      {Tuple::point(2, 2, 0),    Tuple::vector(-1, 0, 0)               }
  };

  for (const auto& [origin, direction] : tests) {
    auto r = Ray(origin, direction);
    auto xs = s->local_intersect(r);
    EXPECT_EQ(0, xs.size());
  }
}

TEST(Cube, CubeNormal) {
  std::shared_ptr<Shape> s;
  s.reset(new Cube());

  std::vector<std::tuple<Tuple, Tuple>> tests{
      {Tuple::point(1, 0.5, -0.8),   Tuple::vector(1, 0, 0)  },
      {Tuple::point(-1, -0.2, 0.9),  Tuple::vector(-1, 0, 0) },
      {Tuple::point(-0.4, 1, -0.1),  Tuple::vector(0, 1, 0)  },
      {Tuple::point(0.3, -1, -0.7),  Tuple::vector(0, -1, 0) },
      {Tuple::point(-0.6, 0.3, 1),   Tuple::vector(0, 0, 1)  },
      {Tuple::point(0.4, 0.4, -1),   Tuple::vector(0, 0, -1) },
      {Tuple::point(1, 1, 1),        Tuple::vector(1, 0, 0)  },
      {Tuple::point(-1, -1, -1),     Tuple::vector(-1, 0, 0) },
  };

  for (const auto& [point, normal] : tests) {
    auto n = s->local_normal_at(point);
    EXPECT_EQ(normal, n);
  }
}

