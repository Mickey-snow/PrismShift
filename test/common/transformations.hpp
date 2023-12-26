#ifndef MY_TRANSFORMATIONS_H
#define MY_TRANSFORMATIONS_H

#include<vector>

#include<util/transform.hpp>
#include<util/geometry.hpp>
#include<util/random.hpp>

static const std::vector<MatrixTransformation> my_transformations{
  MatrixTransformation(),
  MatrixTransformation::Translate(-2.34, 0, 15),
  MatrixTransformation::Rotate(Vector3::Random_Unit(), 2.74),
  MatrixTransformation::RotateFrTo(Vector3::Random_Unit(), Vector3::Random_Unit()),
  MatrixTransformation::Scale(Vector3::Random_Unit()),
  MatrixTransformation::Rotate(Vector3::Random_Unit(),random_uniform_01())*MatrixTransformation::Scale(Vector3::Random(-3,3))
};


#endif
