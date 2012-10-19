#ifndef PARTICLE_H
#define PARTICLE_H

#include "clam.h"
#include <vector>

struct Particle{
	Particle(void):scale(1.0){};
	~Particle(void){};
	clam::vec3d pos;
	unsigned int index;
	double scale;
	unsigned int type;
	//This parameter is not used for spheres
	std::vector<clam::vec3d> vertices;
};

#endif