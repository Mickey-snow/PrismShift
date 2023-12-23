#ifndef SAMPLING_H
#define SAMPLING_H

#include "util/geometry_fwd.hpp"

double pdf_cosine_distributed_hemisphere(const Vector3& wo);

Vector3 Spawn_cosine_distributed_hemisphere();

#endif
