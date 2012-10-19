#ifndef QUATERNION_H
#define QUATERNION_H

#include "clam.h"

typedef struct{
	double el[4];
}quat_t;
void quat_rot(quat_t a, clam::vec3d &b);
#endif