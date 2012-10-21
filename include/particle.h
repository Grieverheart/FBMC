#ifndef PARTICLE_H
#define PARTICLE_H

#include "clam.h"
#include "typedefs.h"
#include <vector>

struct Particle{
	Particle(void):scale(1.0){};
	~Particle(void){};
	clam::vec3d pos;
	uint index;
	double scale;
	uint type;
	//This parameter is not used for spheres
	std::vector<clam::vec3d> vertices;
};

#endif