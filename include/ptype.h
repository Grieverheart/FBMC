#ifndef PTYPE_H
#define PTYPE_H

#include <vector>
#include <string>
#include "clam.h"
#include "typedefs.h"

struct Ptype{
	double iscrb, cscrb;
	uint triadV1, triadV2;
	std::vector<clam::vec3d> vertices;
	std::vector< std::vector<uint> > vertNeigh;
	std::string name;
	
	uint poly_max(std::vector<clam::vec3d> vertices, clam::vec3d axis)const;
	uint poly_min(std::vector<clam::vec3d> vertices, clam::vec3d axis)const;
};


inline uint Ptype::poly_max(std::vector<clam::vec3d> vertices, clam::vec3d axis)const{
	//////////////////////////////////////////////////////
	//  Find the maximum projection on the axis using a //
	//	hill-climbing algorithm							//
	//////////////////////////////////////////////////////
	uint next = 0, last = 0, curr = 0;
	double p = 0.0;
	double max = clam::dot(vertices[0], axis);
	std::vector<uint>::const_iterator it;
	while(1){
		for(it = vertNeigh[curr].begin(); it < vertNeigh[curr].end(); it++){
			next = *it;
			if(next != last){
				p = clam::dot(vertices[next], axis);
				if(p > max){
					max = p;
					last = curr;
					curr = next;
					break;
				}
			}
			if(it == vertNeigh[curr].end() - 1) return curr;
		}
	}
}


inline uint Ptype::poly_min(std::vector<clam::vec3d> vertices, clam::vec3d axis)const{
	//////////////////////////////////////////////////////
	//  Find the maximum projection on the axis using a //
	//	hill-climbing algorithm							//
	//////////////////////////////////////////////////////
	uint next = 0, last = 0, curr = 0;
	double p = 0.0;
	double min = clam::dot(vertices[0], axis);
	std::vector<uint>::const_iterator it;
	while(1){
		for(it = vertNeigh[curr].begin(); it < vertNeigh[curr].end(); it++){
			next = *it;
			if(next != last){
				p = clam::dot(vertices[next], axis);
				if(p < min){
					min = p;
					last = curr;
					curr = next;
					break;
				}
			}
			if(it == vertNeigh[curr].end() - 1) return curr;
		}
	}
}

#endif