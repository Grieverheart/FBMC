#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "particle.h"
#include "ptype.h"
#include "box.h"

class CollisionDetector{
public:
	CollisionDetector(void);
	~CollisionDetector(void);
	void Init(std::vector<Ptype> typesList);
	bool isColliding(Particle a, Particle b, Box box)const;
private:
	bool gjk_overlap(Particle a, Particle b, Box box)const;
	//////////////////////////////GJK Helper Function//////////////////////////////
	clam::vec3d support(Particle a, Particle b, clam::vec3d real_dist, clam::vec3d dir)const;
	
	std::vector<Ptype> types_;
};

#endif