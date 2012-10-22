#include "../include/simulation_base.h"#include <math.h>#include <ctime>#include <iostream>#include <fstream>#include <sstream>#include <unistd.h>#include <boost/random/uniform_int_distribution.hpp>#include <boost/random/uniform_01.hpp>#include "../include/quaternion.h"static bool readProperties(std::string typeName, Ptype &ptype, double scale);static inline double sqr_d(double a){	//////////////////////////////////////	//  Returns the square of a double  //	//////////////////////////////////////	return a*a;}double simulationBase::mtRandd(double x, bool isSymmetric)const{	static boost::random::uniform_01<boost::mt19937> mtUniReal(randGen_);	return isSymmetric? x * (2.0 * mtUniReal() - 1.0) : x * mtUniReal();}int simulationBase::mtRandi(int x){	static boost::random::uniform_int_distribution<> mtUniInt(0);	return mtUniInt(randGen_) % x;}bool simulationBase::Init(int argc, char *argv[]){	uint seed = std::time(0) + getpid();	dr_ = dtheta_ = 0.01;	dv_ = 2.5;	for(int i = 1; i < argc; i++){		if(std::string(argv[i]) == "-seed"){			std::istringstream s(std::string(argv[i+1]));			s >> seed;		}		else if(std::string(argv[i]) == "-dr"){			std::istringstream s(std::string(argv[i+1]));			s >> dr_;		}		else if(std::string(argv[i]) == "-dv"){			std::istringstream s(std::string(argv[i+1]));			s >> dv_;		}		else if(std::string(argv[i]) == "-dtheta"){			std::istringstream s(std::string(argv[i+1]));			s >> dtheta_;		}	}	randGen_.seed(seed);	if(!readConfig(argv[argc-1])) return false;	collisionDetector_.Init(types_);		createImageLists(box_, sil_, eil_);		return true;}void simulationBase::createImageLists(Box box, std::vector<clam::vec3i> &sil, std::vector<clam::vec3i> &eil){	//Find the vertices of the limiting cube in reduced coordinates	if(!sil.empty()) sil.clear();	if(!eil.empty()) eil.clear();	std::vector<clam::vec3d> cubeVerts;	clam::mat3d boxInv = box.inverse();	for(int i = -1; i <= 2; i += 2){		for(int j = -1; j <= 2; j += 2){			for(int k = -1; k <= 2; k += 2){				clam::vec3d cubeVert(i, j, k);				clam::vec3d cubeVertRed;				cubeVertRed[0] = 2.0 * Rmax_ * clam::dot(boxInv.Row(0), cubeVert);				cubeVertRed[1] = 2.0 * Rmax_ * clam::dot(boxInv.Row(1), cubeVert);				cubeVertRed[2] = 2.0 * Rmax_ * clam::dot(boxInv.Row(2), cubeVert);				cubeVerts.push_back(cubeVertRed);			}		}	}	//Find the maxima	std::vector<clam::vec3d>::iterator itr;	clam::vec3d maxVec = cubeVerts[0];	for(itr = cubeVerts.begin() + 1; itr < cubeVerts.end(); itr++){		for(uint i = 0; i < 3; i++){			if((*itr)[i] > maxVec[i]) maxVec[i] = (*itr)[i];		}	}	clam::vec3i maxInts;	for(uint i = 0; i < 3; i++) maxInts[i] = int(ceil(maxVec[i]));	//Create the SIL	clam::vec3i one(1, 1, 1);	for(int i = -maxInts[0]; i <= maxInts[0]; i++){		for(int j = -maxInts[1]; j <= maxInts[1]; j++){			for(int k = -maxInts[2]; k <= maxInts[2]; k++){				if(i == 0 && j == 0 && k == 0) continue;				clam::vec3i point(i, j, k);				if(clam::dot(point, one) > 0.0) sil.push_back(point);			}		}	}	//Create the EIL	for(int m = 0; m < 3; m++){		for(int i = 0; i <= maxInts[m]; i++){				for(int j = 0; j <= maxInts[(m + 1)%3] + 1; j++){				clam::vec3i point;				point[m] = i;				point[(m + 1)%3] = j;				point[(m + 2)%3] = maxInts[(m + 2)%3] + 1;				eil.push_back(point);			}		}	}}void simulationBase::triad(Particle p, double* aa)const{	////////////////////////////////////////////////////	// Find the Axis-Angle Representation using the   //	// Triad method of attitude estimation.			  //	// Algorithm for Angle-Axis conversion can be	  //	// found in "Rotation: ..." by Rebecca M. Brannon //	////////////////////////////////////////////////////	uint TriadV1 = types_[p.type].triadV1;	uint TriadV2 = types_[p.type].triadV2;	double a[3], b[3], c[3], A[3], B[3], C[3], R[9];	double norm_a = 0.0, norm_b = 0.0, norm_c = 0.0;	double norm_A = 0.0, norm_B = 0.0, norm_C = 0.0;		/* Get Orthonormal Basis */		b[0] = 		types_[p.type].vertices[TriadV1][1] * types_[p.type].vertices[TriadV2][2] -		types_[p.type].vertices[TriadV1][2] * types_[p.type].vertices[TriadV2][1];	b[1] =		types_[p.type].vertices[TriadV1][2] * types_[p.type].vertices[TriadV2][0] -		types_[p.type].vertices[TriadV1][0] * types_[p.type].vertices[TriadV2][2];	b[2] = 		types_[p.type].vertices[TriadV1][0] * types_[p.type].vertices[TriadV2][1] -		types_[p.type].vertices[TriadV1][1] * types_[p.type].vertices[TriadV2][0];		c[0] = types_[p.type].vertices[TriadV1][1] * b[2] - types_[p.type].vertices[TriadV1][2] * b[1];	c[1] = types_[p.type].vertices[TriadV1][2] * b[0] - types_[p.type].vertices[TriadV1][0] * b[2];	c[2] = types_[p.type].vertices[TriadV1][0] * b[1] - types_[p.type].vertices[TriadV1][1] * b[0];		B[0] = p.vertices[TriadV1][1] * p.vertices[TriadV2][2] - p.vertices[TriadV1][2] * p.vertices[TriadV2][1];	B[1] = p.vertices[TriadV1][2] * p.vertices[TriadV2][0] - p.vertices[TriadV1][0] * p.vertices[TriadV2][2];	B[2] = p.vertices[TriadV1][0] * p.vertices[TriadV2][1] - p.vertices[TriadV1][1] * p.vertices[TriadV2][0];		C[0] = p.vertices[TriadV1][1] * B[2] - p.vertices[TriadV1][2] * B[1];	C[1] = p.vertices[TriadV1][2] * B[0] - p.vertices[TriadV1][0] * B[2];	C[2] = p.vertices[TriadV1][0] * B[1] - p.vertices[TriadV1][1] * B[0];		for(uint i = 0; i < 3; i++){		norm_a += sqr_d(types_[p.type].vertices[TriadV1][i]);		norm_b += sqr_d(b[i]);		norm_c += sqr_d(c[i]);		norm_A += sqr_d(p.vertices[TriadV1][i]);		norm_B += sqr_d(B[i]);		norm_C += sqr_d(C[i]);	}	norm_a = sqrt(norm_a);	norm_b = sqrt(norm_b);	norm_c = sqrt(norm_c);	norm_A = sqrt(norm_A);	norm_B = sqrt(norm_B);	norm_C = sqrt(norm_C);		for(uint i = 0; i < 3; i++){		a[i] = types_[p.type].vertices[TriadV1][i] / norm_a;		b[i] /= norm_b;		c[i] /= norm_c;		A[i] = p.vertices[TriadV1][i] / norm_A;		B[i] /= norm_B;		C[i] /= norm_C;	}		/* Calculate the Rotation Matrix */		for(uint i = 0; i < 3; i++){		for(uint j = 0; j < 3; j++) R[3*i+j] = A[i] * a[j] + B[i] * b[j] + C[i] * c[j];	}	/* Convert Rotation Matrix to Angle-Axis Representation */	double t = R[0] + R[4] + R[8];	double cos_a = 0.5 * (t - 1.0);	double diff_cos = sqrt(1.0 - sqr_d(cos_a));	aa[0] = acos(cos_a);	if(diff_cos > 0.1e-10){		double ss = 1.0 / (2.0 * sin(aa[0]));		aa[1] = ss * (R[7] - R[5]);		aa[2] = ss * (R[2] - R[6]);		aa[3] = ss * (R[3] - R[1]);	}	else if(cos_a > 0.0){		aa[1] = 1.0;		aa[2] = 0.0;		aa[3] = 0.0;	}	else{		uint column = 0;		double norm;		for(uint i = 0; i < 3; i++){			norm = 0.0;			uint flag = 0;			for(uint j = 0; j < 3; j++){				if(R[3*j+i] == 0.0) flag += 1;				else norm += sqr_d(R[3*j+i]);			}			if(flag < 3){				column = i;				break;			}		}		norm = sqrt(norm);				aa[0] = M_PI;		aa[1] = R[column] / norm;		aa[2] = R[3+column] / norm;		aa[3] = R[6+column] / norm;			}	aa[0] = aa[0] * 180.0 / M_PI;}bool simulationBase::saveConfig(std::string filename)const{	std::string filepath = "Data/"+filename;	std::ofstream file(filepath.c_str());	if(!file){		std::cout << "Error creating file \"" << filepath << "\" by function " << "'" << __FUNCTION__ << "'" << std::endl;		return false;	}		file << nPart_ << std::endl;	file << types_.size() << std::endl;		file.setf(std::ios::fixed, std::ios::floatfield);	file.precision(6);		for(uint i = 0; i < 9; i++){		file << box_[i] << "\t";	}	file << std::endl;		for(uint i = 0; i < nPart_; i++){		file << particles_[i].type << "\t";		for(uint j = 0; j < 3; j++)	file << clam::dot(box_.Row(j), particles_[i].pos) << "\t";		double Rot[4] = {0.0};		if(types_[particles_[i].type].name != "Sphere") triad(particles_[i], Rot);		else Rot[1] = 1.0;		for(uint j = 0; j < 4; j++) file << Rot[j] << "\t";		file << std::endl;	}		for(uint i = 0; i < types_.size(); i++) file << types_[i].name << std::endl;		file.close();	return true;}bool simulationBase::readConfig(const char *filepath){	uint nTypes;	std::vector<quat_t> rotations;		std::ifstream file(filepath);	if(!file){		std::cout << "Error parsing file \"" << filepath << "\" by function " << "'" << __FUNCTION__ << "'" << std::endl;		return false;	}		std::cout << "Parsing Configuration File: '" << std::string(filepath) << "'" << std::endl;		std::string line;	std::getline(file, line);	std::istringstream s(line);		//Read number of particles	s >> nPart_;	std::cout << "Found " << nPart_ << " particles." << std::endl;		s.clear();	std::getline(file, line);	s.str(line);		//Read number of particle types	s >> nTypes;		s.clear();	std::getline(file, line);	s.str(line);		//Read Box properties	for(uint i = 0; i < 9; i++) s >> box_[i];	clam::mat3d boxInverse(box_.inverse());		//Read Particle position, type and rotation	for(uint i = 0; i < nPart_; i++){		std::getline(file, line);		std::istringstream stream(line);		Particle tempParticle;		quat_t tempRot;		stream >> tempParticle.type >> std::ws;		stream >> tempParticle.pos[0] >> tempParticle.pos[1] >> tempParticle.pos[2] >> std::ws;		for(uint j = 0; j < 3; j++) tempParticle.pos[j] = clam::dot(boxInverse.Row(j), tempParticle.pos);		stream >> tempRot.el[0] >> tempRot.el[1] >> tempRot.el[2] >> tempRot.el[3] >> std::ws;		rotations.push_back(tempRot);		tempParticle.index = i;		particles_.push_back(tempParticle);	}		//Read type names and initiate types	for(uint i = 0; i < nTypes; i++){		std::getline(file, line);		std::istringstream stream(line);		std::string typeName;		double scale = 1.0;		stream >> typeName >> std::ws;		if(!stream.eof()) stream >> scale >> std::ws;		Ptype tempType;		if(typeName != "Sphere"){			if(!readProperties(typeName, tempType, scale)) return false;		}		else tempType.name = "Sphere";		types_.push_back(tempType);	}	file.close();		//Rotate each particle's vertices acording to the rotations read from the input file	//Also find the maximum circumscribed radius or just radius in case of spheres	double max = 0.0;	for(uint i = 0; i < nPart_; i++){		uint partType = particles_[i].type;		if(types_[partType].name != "Sphere"){			particles_[i].vertices = types_[partType].vertices;			quat_t q_rotation;			rotations[i].el[0] *= M_PI / 180.0;			q_rotation.el[0] = cos(0.5 * rotations[i].el[0]);			q_rotation.el[1] = rotations[i].el[1] * sin(0.5 * rotations[i].el[0]);			q_rotation.el[2] = rotations[i].el[2] * sin(0.5 * rotations[i].el[0]);			q_rotation.el[3] = rotations[i].el[3] * sin(0.5 * rotations[i].el[0]);			std::vector<clam::vec3d>::iterator itr;			for(itr = particles_[i].vertices.begin(); itr < particles_[i].vertices.end(); itr++){				quat_rot(q_rotation, *itr);			}			if(particles_[i].scale * types_[partType].cscrb > max) max = types_[partType].cscrb;		}		else if(particles_[i].scale > max) max = particles_[i].scale;	}	Rmax_ = max;	std::cout << "Successfully done parsing." << std::endl;	std::cout << std::endl;	return true;}static bool readProperties(std::string typeName, Ptype &ptype, double scale = 1.0){	std::cout << "Parsing Properties for " << typeName << "." << std::endl;	ptype.name = typeName;	std::vector<clam::vec3d> vertices;	std::vector< std::vector<uint> > faces;	std::vector< std::vector<uint> > edges;	std::vector< std::vector<uint> > vertNeigh;		/* Parse File */	const char *filepath = ("obj/" + typeName + ".obj").c_str();	std::ifstream file(filepath);	if(!file){		std::cout << "Error parsing file \"" << filepath << "\" by function " << "'" << __FUNCTION__ << "'" << std::endl;		return false;	}		std::string line;	while(std::getline(file, line)){		if(line[0] == 'v'){			std::istringstream s(line.substr(2));			clam::vec3d vertex;			s >> vertex[0] >> vertex[1] >> vertex[2] >> std::ws;			for(int i = 0; i < 3; i++) vertex[i] *= scale;			vertices.push_back(vertex);		}		else if(line[0] == 'f'){			std::istringstream s(line.substr(2));			std::vector<uint> face;			uint faceVertexIndex;			while(s >> faceVertexIndex) face.push_back(faceVertexIndex - 1);			faces.push_back(face);		}	}	file.close();	ptype.vertices = vertices;	std::cout << "Found " << vertices.size() << " vertices." << std::endl;		/* Calculate Properties */		//Find Edges	std::vector< std::vector<uint> >::const_iterator faceItr1, faceItr2;	//Iterate over each face and for each next face in the list, check if they 	//share two vertices, this defines an edge.	for(faceItr1 = faces.begin(); faceItr1 < faces.end(); faceItr1++){		for(faceItr2 = faceItr1 + 1; faceItr2 < faces.end(); faceItr2++){			uint fcount = 0;			std::vector<uint> edge;			std::vector<uint>::const_iterator itr1, itr2;			for(itr1 = (*faceItr1).begin(); itr1 < (*faceItr1).end(); itr1++){				for(itr2 = (*faceItr2).begin(); itr2 < (*faceItr2).end(); itr2++){					if(*itr1 == *itr2){						edge.push_back(*itr1);						fcount++;					}				}				if(fcount == 2){					edges.push_back(edge);					fcount = 0;					edge.clear();				}			}		}	}	std::cout << "Found " << edges.size() << " edges." << std::endl;		//Find Vertex Neighbours	//For all vertices, check if two edges share this vertex. If they do and it	//isn't vertex 0, append the other vertices of these edge to the neighbor list	for(uint vi = 0; vi < vertices.size(); vi++){		std::vector<uint> neighbors;		for(uint ei = 0; ei < edges.size(); ei++){			for(uint i = 0; i < 2; i++){				if(edges[ei][i] == vi){					if(edges[ei][(i+1)%2] != 0) neighbors.push_back(edges[ei][(i+1)%2]);				}			}		}		if(!neighbors.empty()) vertNeigh.push_back(neighbors);	}	ptype.vertNeigh = vertNeigh;		//Find the inRadius	//For each face, calculate its distance from the particle's center and find the min	std::vector< std::vector<uint> >::const_iterator faceItr;	double minDistance = 100.0;	for(faceItr = faces.begin(); faceItr < faces.end(); faceItr++){		clam::vec3d p(vertices[(*faceItr)[0]]);				clam::vec3d a(			vertices[(*faceItr)[1]][0] - vertices[(*faceItr)[0]][0],			vertices[(*faceItr)[1]][1] - vertices[(*faceItr)[0]][1],			vertices[(*faceItr)[1]][2] - vertices[(*faceItr)[0]][2]		);				clam::vec3d b(			vertices[(*faceItr)[2]][0] - vertices[(*faceItr)[0]][0],			vertices[(*faceItr)[2]][1] - vertices[(*faceItr)[0]][1],			vertices[(*faceItr)[2]][2] - vertices[(*faceItr)[0]][2]		);				clam::vec3d normal = a.cross(b);		double length = clam::length(normal);		for(int i = 0; i < 3; i++) normal[i] /= length;		double faceDistance = fabs(clam::dot(normal, p));				if(faceDistance < minDistance) minDistance = faceDistance;	}	ptype.iscrb = minDistance;	std::cout << "Inscribed Radius: " << ptype.iscrb << std::endl;		//Find the circumRadius	//It's just the farthest vertex from the particle's center	double maxDistance = 0.0;	std::vector<clam::vec3d>::const_iterator vItr;	for(vItr = vertices.begin(); vItr < vertices.end(); vItr++){		double vertexLength = clam::length(*vItr);		if( vertexLength > maxDistance) maxDistance = vertexLength;	}	ptype.cscrb = maxDistance;	std::cout << "Circumscribed Radius: " << ptype.cscrb << std::endl;		//Find two linearly independent vertices	ptype.triadV1 = 1;	for(uint i = 1; i < vertices.size(); i++){		double crossLength = clam::length(vertices[ptype.triadV1].cross(vertices[i]));		if(crossLength > 1.0e-10){			ptype.triadV2 = i;			break;		}	}	std::cout << "Triad V1: " << ptype.triadV1 << ", Triad V2: " << ptype.triadV2 << std::endl;	std::cout << std::endl;		return true;}