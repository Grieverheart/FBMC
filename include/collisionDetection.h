#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "particle.h"
#include "ptype.h"

class CollisionDetector{
public:
	CollisionDetector(void);
	~CollisionDetector(void);
	void Init(std::vector<Ptype> typesList);
	bool isColliding(Particle const &a, Particle const &b)const;
private:
	double* axesCache_;
	std::vector<Ptype> types;
};

#endif