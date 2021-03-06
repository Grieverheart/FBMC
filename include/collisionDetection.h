#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "particle.h"
#include "ptype.h"
#include "box.h"
#include "clam.h"

class CollisionDetector{
public:
	CollisionDetector(void){};
	~CollisionDetector(void){};
	void Init(std::vector<Ptype> typesList);
	bool isColliding(Particle a, Particle b, Box box)const;
private:
	double distance(clam::vec3d, clam::vec3d, Box box)const; //distance squared
	bool gjk_overlap(Particle a, Particle b, Box box)const;
	//////////////////////////////GJK Helper Function//////////////////////////////
	clam::vec3d support(Particle a, Particle b, clam::vec3d real_dist, clam::vec3d dir)const;
	
	std::vector<Ptype> types_;
};


inline double CollisionDetector::distance(clam::vec3d a, clam::vec3d b, Box box)const{
	clam::vec3d distance;
	clam::vec3d result;
	for(uint i = 0; i < 3; i++) distance[i] = a[i] - b[i];
	for(uint i = 0; i < 3; i++) result[i] = clam::dot(box.Row(i), distance);
	return clam::length(result);
}

#endif