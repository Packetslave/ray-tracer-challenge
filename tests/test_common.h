//
// Created by Brian Landers on 8/7/20.
//

#pragma once

#include <cmath>

#include "../core/matrix.h"
#include "../core/tuple.h"

bool vector_is_near(Tuple a, Tuple b, double abs);
bool tuple_is_near(Tuple a, Tuple b);
bool matrix_is_near(Matrix a, Matrix b, double abs);
bool point_is_near(Tuple a, Tuple b, double abs);