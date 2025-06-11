#pragma once

#include <vector>

#include <util/vector.hpp>
#include <util/random.hpp>
#include <util/transform.hpp>

static const std::vector<MatrixTransformation> my_transformations{
    MatrixTransformation(),
    MatrixTransformation::Translate(-2.34, 0, 15),
    MatrixTransformation::Rotate(Vector3::Random_Unit(), 2.74),
    MatrixTransformation::RotateFrTo(Vector3::Random_Unit(),
                                     Vector3::Random_Unit()),
    MatrixTransformation::Scale(Vector3::Random_Unit()),
    MatrixTransformation::Rotate(Vector3::Random_Unit(), random_uniform_01()) *
        MatrixTransformation::Scale(Vector3::Random(-3, 3))};
