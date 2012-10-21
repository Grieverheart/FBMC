#ifndef QUATERNION_H
#define QUATERNION_H

#include "clam.h"

typedef struct{
	double el[4];
}quat_t;

inline void quat_rot(quat_t a, clam::vec3d &b){
	///////////////////////////////////////////
	// Multiply vector b by quaternion a	 //
	///////////////////////////////////////////
	double cross_temp[3], result[3];
	
	cross_temp[0] = a.el[2] * b[2] - a.el[3] * b[1] + a.el[0] * b[0];
	cross_temp[1] = a.el[3] * b[0] - a.el[1] * b[2] + a.el[0] * b[1];
	cross_temp[2] = a.el[1] * b[1] - a.el[2] * b[0] + a.el[0] * b[2];
	
	result[0] = b[0] + 2.0 * (a.el[2] * cross_temp[2] - a.el[3] * cross_temp[1]);
	result[1] = b[1] + 2.0 * (a.el[3] * cross_temp[0] - a.el[1] * cross_temp[2]);
	result[2] = b[2] + 2.0 * (a.el[1] * cross_temp[1] - a.el[2] * cross_temp[0]);
	
	for(unsigned int i = 0; i < 3; i++) b[i] = result[i];
}

#endif