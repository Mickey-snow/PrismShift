#ifndef SAMPLING_H
#define SAMPLING_H

class Vector3;

double pdf_cosine_distributed_hemisphere(const Vector3& wo);

Vector3 Spawn_cosine_distributed_hemisphere();

#endif
