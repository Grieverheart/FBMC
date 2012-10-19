#include "../include/collisionDetection.h"

void CollisionDetector::Init(std::vector<Ptype> typesList){
}

bool CollisionDetector::isColliding(Particle const &a, Particle const &b)const{
	if(types[a.type].name == "Sphere" && types[b.type].name == "Sphere") return true;
	return false;
}