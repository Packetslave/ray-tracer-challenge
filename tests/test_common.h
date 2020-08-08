//
// Created by Brian Landers on 8/7/20.
//

#pragma once

class Tuple;
class Matrix;

bool vector_is_near(Tuple a, Tuple b, float abs);
bool tuple_is_near(Tuple a, Tuple b);
bool matrix_is_near(Matrix a, Matrix b, float abs);
bool point_is_near(Tuple a, Tuple b, float abs);