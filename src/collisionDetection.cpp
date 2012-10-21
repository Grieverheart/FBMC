#include "../include/collisionDetection.h"
#include <iostream>

void CollisionDetector::Init(std::vector<Ptype> typesList){
	types_ = typesList;
}

bool CollisionDetector::isColliding(Particle a, Particle b, Box box)const{
	if(types_[a.type].name == "Sphere" && types_[b.type].name == "Sphere") return true;
	return false;
}



typedef std::vector<clam::vec3d> Simplex; 

static inline clam::vec3d triple_p(clam::vec3d a, clam::vec3d b,  clam::vec3d c){
	//////////////////////////////
	// Triple product (axb)xc 	//
	//////////////////////////////
	double dot_ac, dot_bc; 
	clam::vec3d result;
	
	dot_ac = clam::dot(a, c); 
	dot_bc = clam::dot(b, c); 
	for(uint i = 0; i < 3; i++){
		result[i] = b[i] * dot_ac - a[i] * dot_bc; 
	}
	return result;
}

static inline clam::vec3d dir_cross_p(clam::vec3d a, clam::vec3d b, clam::vec3d direction){
	//////////////////////////////////////////
	//  Cross product with the sign of c 	//
	//////////////////////////////////////////
	clam::vec3d result;
	
	result = a.cross(b);
	if(clam::dot(result, direction) > 0.0){
		for(uint i = 0; i < 3; i++) result[i] = -result[i]; 
	}
	return result;
}

static inline clam::vec3d transformDistance(clam::vec3d a_center, clam::vec3d b_center, Box box){
	//////////////////////////////////////////////////////
	//  Convert the distance between the particles to 	//
	//	real coordinates.								//
	//////////////////////////////////////////////////////

	clam::vec3d dist; 
	clam::vec3d result;
	
	for(uint i = 0; i < 3; i++){
		dist[i] = a_center[i] - b_center[i]; 
	}
	
	/* Convert the center to real coordinates */
	for(uint i = 0; i < 3; i++){
		result[i] = clam::dot(box.Row(i), dist);
	}
	
	return result;
}

inline clam::vec3d CollisionDetector::support(Particle a, Particle b, clam::vec3d real_dist, clam::vec3d dir)const{
	//////////////////////////////////////////////////////
	//  The support function. This provides us with 	//
	//	the next point for our simplex.					//
	//////////////////////////////////////////////////////
	uint pa, pb;
	clam::vec3d suppA, suppB;
	std::string aName = types_[a.type].name;
	std::string bName = types_[b.type].name;
	if(aName == "Sphere"){
		double norm = clam::length(dir);
		for(uint i = 0; i < 3; i++) suppA[i] = a.scale * dir[i] / norm;
		pb = types_[b.type].poly_min(b.vertices, dir);
		suppB = b.vertices[pb];
	}
	else if(bName == "Sphere"){
		double norm = clam::length(dir);
		for(uint i = 0; i < 3; i++) suppB[i] = b.scale * dir[i] / norm;
		pa = types_[a.type].poly_max(a.vertices, dir);
		suppA = a.vertices[pa];
	}
	else{
		pa = types_[a.type].poly_max(a.vertices, dir);
		suppA = a.vertices[pa];
		pb = types_[b.type].poly_min(b.vertices, dir);
		suppB = b.vertices[pb];
	}
	
	clam::vec3d c;
	
	for(uint i = 0; i < 3; i++){
		c[i] = real_dist[i] + suppA[i] - suppB[i]; 
	}
	
	return c;
}

static bool containsOrigin(Simplex &s, clam::vec3d &dir){
	///////////////////////////////////////////////
	//  Check if the origin is contained in the  //
	//	Minkowski sum. 							 //
	///////////////////////////////////////////////
	clam::vec3d a = s[s.size() - 1]; 
	clam::vec3d b = s[s.size() - 2]; 
	if(s.size() == 4){
		/* Tetrahedron Case */
		clam::vec3d c = s[1]; //[dcba]
		clam::vec3d d = s[0]; 
		clam::vec3d ab, ac, ad; 
		clam::vec3d abcPerp, acdPerp, abdPerp; 
		
		for(uint i = 0; i < 3; i++){
			ab[i] = b[i]-a[i]; 
			ac[i] = c[i]-a[i]; 
			ad[i] = d[i]-a[i]; 
		}
		/* Face Normal facing outwards */
		abcPerp = dir_cross_p(ab, ac, ad); 
		acdPerp = dir_cross_p(ac, ad, ab); 
		abdPerp = dir_cross_p(ab, ad, ac); 
		
		bool Oabc = false; 
		bool Oabd = false; 
		bool Oacd = false; 
		
		/* Origin on which Side? */
		if(clam::dot(abcPerp, a) < 0.0) Oabc = true; 
		if(clam::dot(abdPerp, a) < 0.0) Oabd = true; 
		if(clam::dot(acdPerp, a) < 0.0) Oacd = true; 
		
		if(!(Oabc && Oabd) && Oacd){
			/* Only on acd side */
			/* Triangle Case */
			s.erase(s.begin() + 2); //erase 3rd element
			dir = acdPerp;
		}
		else if(!(Oabc && Oacd) && Oabd){
			/* Only on abd side */
			/* Triangle Case */
			s.erase(s.begin() + 1); //erase 2nd element
			dir = abdPerp;
		}
		else if(!(Oabd && Oacd) && Oabc){
			/* Only on abc side */
			/* Triangle Case */
			s.erase(s.begin() + 0); //erase 1st element
			dir = abcPerp;
		}
		else if(!Oacd && (Oabc && Oabd)){
			/* Only on abc and abd */
			/*Line Case*/
			dir = triple_p(a, ab, ab);
			s.erase(s.begin(), s.begin() + 2); //Erase first two elements
		}
		else if(!Oabd && (Oabc && Oacd)){
			/* Only on abc and acd */
			/*Line Case*/
			dir = triple_p(a, ac, ac); 
			s.erase(s.begin() + 2); //erase 3rd element
			s.erase(s.begin() + 0); //erase 1st element
		}
		else if(!Oabc && (Oacd && Oabd)){
			/* Only on acd and abd */
			/*Line Case*/
			dir = triple_p(a, ad, ad); 
			s.erase(s.begin() + 1, s.begin() + 3); //Erase middle two elements
		}
		/* 'else' should only be when the origin is inside the tetrahedron */
		else return true; 
	}
	else if(s.size() == 3){
		/* Triangle Case */
		clam::vec3d c = s[0]; //[cba]
		clam::vec3d ab, ac; 
		clam::vec3d abPerp, acPerp; 
		for(uint i = 0; i < 3; i++){
			ab[i] = b[i] - a[i]; 
			ac[i] = c[i] - a[i]; 
		}
		abPerp = triple_p(ac, ab, ab); 
		acPerp = triple_p(ab, ac, ac); 
		/* Origin on the outside of triangle and close to ab */
		/* Remove Point c */
		if(clam::dot(abPerp, a) < 0.0){
			s.erase(s.begin() + 0);
			dir = abPerp; //Concider Taking triple_p(a, ab, ab)
		}
		/* Origin on the outside of triangle and close to ac */
		/* Remove Point b */
		else if(clam::dot(acPerp, a) < 0.0){
			s.erase(s.begin() + 1);
			dir = acPerp; //Concider Taking triple_p(a, ac, ac)
		}
		/* Origin's projection on the inside of triangle. */
		/* Take perpendicular to triangle */
		else{
			dir = dir_cross_p(ab, ac, a); 
		}
	}
	else if(s.size() == 2){
		/*Line Case*/
		clam::vec3d ab, abPerp; 
		for(uint i = 0; i < 3; i++){
			ab[i] = b[i] - a[i]; 
		}
		abPerp = triple_p(a, ab, ab);
		dir = abPerp;
	}
	else{
		/*Point Case*/
		for(uint i = 0; i < 3; i++){
			dir[i] = -a[i]; 
		}
		return false; 
	}
	return false; 
}

bool CollisionDetector::gjk_overlap(Particle a, Particle b, Box box)const{
	
	clam::vec3d real_distance = transformDistance(a.pos, b.pos, box); 
	
	/////////////////////////GJK Starts Here//////////////////////////////////
	clam::vec3d dir; 
	Simplex S;
	
	dir = clam::vec3d(1.0, 0.0, 0.0); 
	
	uint fail_safe  =  0; 
	while(fail_safe < 30){
	
		fail_safe++; 
		
		clam::vec3d suppVec = support(a, b, real_distance, dir);
		S.push_back(suppVec);
		
		if(clam::dot(S.back(), dir) < 0.0) return false; 
		else if(containsOrigin(S, dir))return true; 
	}
	std::cout << "Encountered error in GJK: Infinite Loop.\n Direction (" << dir[0] << ", " << dir[1] << ", " << dir[2] << std::endl; 
	for(uint i = 0;  i  <  S.size();  i++){
		std::cout << S[i][0] << "\t" <<  S[i][1] << "\t" << S[i][2] << std::endl; 
	}
	return true; 
}