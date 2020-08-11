#include "test_common.h"

#include "../core/matrix.h"
#include "../core/tuple.h"

bool vector_is_near(Tuple a, Tuple b, double abs) {
  std::cout << "A: " << a << std::endl;
  std::cout << "B: " << b << std::endl;

  return ((std::abs(a.x - b.x) < abs) && (std::abs(a.y - b.y) < abs) &&
          (std::abs(a.z - b.z) < abs) && (a.w == b.w));
}

bool tuple_is_near(Tuple a, Tuple b) {
  return abs(a.x - b.x) < EPSILON && abs(a.y - b.y) < EPSILON &&
         abs(a.x - b.x) < EPSILON && abs(a.w - b.w) < EPSILON;
}

bool matrix_is_near(Matrix a, Matrix b, double abs) {
  for (size_t row = 0; row < a.size(); ++row) {
    for (size_t col = 0; col < a.size(); ++col) {
      if (std::abs(a.get(row, col) - b.get(row, col)) > abs) {
        return false;
      }
    }
  }
  return true;
}

bool point_is_near(Tuple a, Tuple b, double abs) {
  std::cout << "A: " << a << std::endl;
  std::cout << "B: " << b << std::endl;

  return ((std::abs(a.x - b.x) < abs) && (std::abs(a.y - b.y) < abs) &&
          (std::abs(a.z - b.z) < abs) && (a.w == b.w));
}
