#ifndef PTYPE_H
#define PTYPE_H

#include <vector>
#include <string>
#include "clam.h"

typedef unsigned int uint;

class Ptype{
public:
	double iscrb_d2, cscrb_d2;
	uint triadV1, triadV2;
	std::vector<clam::vec3d> vertices;
	std::vector< std::vector<uint> > vertNeigh;
	std::string name;
	
	uint poly_max(std::vector<clam::vec3d> vertices, clam::vec3d axis);
	uint poly_min(std::vector<clam::vec3d> vertices, clam::vec3d axis);
};

#endif