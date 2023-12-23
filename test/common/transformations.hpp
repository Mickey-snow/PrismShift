#ifndef MY_TRANSFORMATIONS_H
#define MY_TRANSFORMATIONS_H

#include<vector>

#include<util/transform.hpp>
#include<util/geometry.hpp>
#include<util/random.hpp>

static const std::vector<Transformation> my_transformations{
  Transformation(),
  Transformation::Translate(-2.34, 0, 15),
  Transformation::Rotate(Vector3::Random_Unit(), 2.74),
  Transformation::RotateFrTo(Vector3::Random_Unit(), Vector3::Random_Unit()),
  Transformation::Scale(Vector3::Random_Unit()),
  Transformation::Rotate(Vector3::Random_Unit(),random_uniform_01())*Transformation::Scale(Vector3::Random(-3,3))
};


#endif
