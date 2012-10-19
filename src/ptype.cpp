#include "../include/ptype.h"


uint Ptype::poly_max(std::vector<clam::vec3d> vertices, clam::vec3d axis){
	//////////////////////////////////////////////////////
	//  Find the maximum projection on the axis using a //
	//	hill-climbing algorithm							//
	//////////////////////////////////////////////////////
	uint next = 0, last = 0, curr = 0;
	double p = 0.0;
	double max = clam::dot(vertices[0], axis);
	std::vector<uint>::iterator it;
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


uint Ptype::poly_min(std::vector<clam::vec3d> vertices, clam::vec3d axis){
	//////////////////////////////////////////////////////
	//  Find the maximum projection on the axis using a //
	//	hill-climbing algorithm							//
	//////////////////////////////////////////////////////
	uint next = 0, last = 0, curr = 0;
	double p = 0.0;
	double min = clam::dot(vertices[0], axis);
	std::vector<uint>::iterator it;
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