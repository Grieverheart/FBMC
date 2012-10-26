#include "../include/simulation_base.h"#include <cmath>#include <ctime>#include <iostream>#include <fstream>#include <sstream>#include <unistd.h>#include <vector>#include <boost/random/mersenne_twister.hpp>#include <boost/random/uniform_int_distribution.hpp>#include <boost/random/uniform_01.hpp>#include "../include/box.h"#include "../include/particle.h"#include "../include/ptype.h"#include "../include/collisionDetection.h"#include "../include/typedefs.h"#include "../include/quaternion.h"#ifndef M_PI#define M_PI	3.14159265358979323846#endifnamespace fbmc{static bool readProperties(std::string typeName, Ptype &ptype, double scale);struct Simulation::Impl{	void triad(Particle p, double *aa)const;	void createImageLists(Box box, std::vector<clam::vec3i> &sil, std::vector<clam::vec3i> &eil)const;	bool readIniFile(const char*);		Box box_;	std::vector<clam::vec3i> sil_;	std::vector<clam::vec3i> eil_;	std::vector<Particle> particles_;	std::vector<Ptype> types_;	uint nPart_;	double dr_, dtheta_, dv_, ds_, bp_;	double Rmax_; //Maximum double radius	boost::random::mt19937 randGen_;	CollisionDetector collisionDetector_;};Simulation::~Simulation(){}Simulation::Simulation():	pimpl_(new Impl()){}static inline double sqr_d(double a){	//////////////////////////////////////	//  Returns the square of a double  //	//////////////////////////////////////	return a*a;}double Simulation::mtRandd(double x, bool isSymmetric)const{	static boost::random::uniform_01<boost::mt19937> mtUniReal(pimpl_->randGen_);	return isSymmetric? x * (2.0 * mtUniReal() - 1.0) : x * mtUniReal();}int Simulation::mtRandi(int x){	static boost::random::uniform_int_distribution<> mtUniInt(0);	return mtUniInt(pimpl_->randGen_) % x;}double Simulation::getVolume(void)const{	return pimpl_->box_.Volume();}double Simulation::getProperty(eProperties property)const{	static bool antiSpam = false;	if(property == MaxTranslation) return pimpl_->dr_;	else if(property == MaxRotation) return pimpl_->dtheta_;	else if(property == MaxVolume) return pimpl_->dv_;	else if(property == MaxDeformation) return pimpl_->ds_;	else if(property == Pressure) return pimpl_->bp_;	else{		if(!antiSpam){			std::cout << "Passed wrong argument to " << __FUNCTION__ << ". Returning -1.0!" << std::endl;			antiSpam = true;		}		return -1.0;	}}void Simulation::setProperty(eProperties property, double value){	static bool antiSpam = false;	if(property == MaxTranslation) pimpl_->dr_ = value;	else if(property == MaxRotation) pimpl_->dtheta_ = value;	else if(property == MaxVolume) pimpl_->dv_ = value;	else if(property == MaxDeformation) pimpl_->ds_ = value;	else if(property == Pressure) pimpl_->bp_ = value;	else if(!antiSpam){		std::cout << "Passed wrong argument to " << __FUNCTION__ << ". Returning -1.0!" << std::endl;		antiSpam = true;	}}inline bool Simulation::Impl::readIniFile(const char *filepath){	std::ifstream f(filepath);	if(!f){		std::cout << "Couldn't open " << filepath << "." << std::endl;		return false;	}	std::string line;	while(std::getline(f, line)){		static uint l = 0;		l++;		std::istringstream s(line);		std::string arg;		double temp;		s >> arg >> temp >> std::ws;		if(arg == "dr"){			dr_ = temp;			std::cout << "Inititalized maximum displacement to " << dr_ << "." << std::endl;		}		else if(arg == "ds"){			ds_ = temp;			std::cout << "Inititalized maximum deformation to " << dr_ << "." << std::endl;		}		else if(arg == "dv"){			dv_ = temp;			std::cout << "Inititalized maximum volume difference to " << dr_ << "." << std::endl;		}		else if(arg == "dtheta"){			dtheta_ = temp;			std::cout << "Inititalized maximum rotation to " << dr_ << "." << std::endl;		}		else if(arg == "bp"){			bp_ = temp;			std::cout << "Inititalized pressure to " << bp_ << "." << std::endl;		}		else{			std::cout << "Encountered error in configuration file " << filepath << " at line " << l << "." << std::endl;			std::cout << "Reading default configuration file instead." << std::endl;			return false;		}	}	f.close();	return true;}bool Simulation::Init(int argc, char *argv[]){	uint seed = std::time(0) + getpid();	bool isIniRead = false;		for(int i = 1; i < argc; i++){		if(std::string(argv[i]) == "-seed"){			std::istringstream s(std::string(argv[i+1]));			s >> seed;		}		else if(std::string(argv[i]) == "-dr"){			std::istringstream s(std::string(argv[i+1]));			s >> pimpl_->dr_;		}		else if(std::string(argv[i]) == "-dv"){			std::istringstream s(std::string(argv[i+1]));			s >> pimpl_->dv_;		}		else if(std::string(argv[i]) == "-dtheta"){			std::istringstream s(std::string(argv[i+1]));			s >> pimpl_->dtheta_;		}		else if(std::string(argv[i]) == "-ds"){			std::istringstream s(std::string(argv[i+1]));			s >> pimpl_->ds_;		}		else if(std::string(argv[i]) == "-f"){			std::ifstream file(argv[i+1]);			if(!file){				std::cout << "Error reading Particle Configuration from file " << argv[i+1] << "." << std::endl;				return false;			}			if(!readConfig(file)) return false;			file.close();		}		else if(std::string(argv[i]) == "-i"){			isIniRead = pimpl_->readIniFile(argv[i+1]);		}	}	if(!isIniRead) pimpl_->readIniFile("config.ini");	pimpl_->randGen_.seed(seed);		return true;}static inline std::vector<Box> genBoxes(Box box){	std::vector<Box> boxList;	for(uint i = 0; i < 3; i++){ //Iterate over all 12 boxes		for(uint j = 0; j < 3; j++){			if(j != i){				for(uint m = 0; m < 2; m++){					double sign = 1.0 - 2.0 * m;					Box tempBox(box);					tempBox[0 + i] = tempBox[0 + i] + sign * box[0 + j];					tempBox[3 + i] = tempBox[3 + i] + sign * box[3 + j];					tempBox[6 + i] = tempBox[6 + i] + sign * box[6 + j];					boxList.push_back(tempBox);				}			}		}	}	return boxList;}void Simulation::latticeReduction(double threshold){	double distortion = (1.0/9.0) * (pimpl_->box_.Area() / pimpl_->box_.Volume()) * (clam::length(pimpl_->box_.Col(0)) + clam::length(pimpl_->box_.Col(1)) + clam::length(pimpl_->box_.Col(2)));	bool isReduced = false;	if(distortion > threshold){		double minArea = pimpl_->box_.Area();		double curArea = minArea;		for(uint i = 0; i < 10; i++){ //10 maximum iteration depth			std::vector<Box> boxList = genBoxes(pimpl_->box_); //Generate box candidates			uint minIndex = 0;			for(uint i = 0; i < 12; i++){ //Iterate over all 12 box candidates				double area = boxList[i].Area();				if(area < minArea){ //If a box has less area, note it					minArea = area;					minIndex = i;				}			}			if(minArea < curArea){ //If the area became less than the last iteration, save the box				curArea = minArea;				pimpl_->box_ = boxList[minIndex];				isReduced = true;			}//Else end the reduction			else break;		}	}	if(isReduced) pimpl_->createImageLists(pimpl_->box_, pimpl_->sil_, pimpl_->eil_); //Is this needed?}bool Simulation::translation(void){	uint nPart_ = pimpl_->nPart_;	uint randPart = mtRandi(nPart_);	Particle tempPart(pimpl_->particles_[randPart]);		//Randomly move particle copy	for(uint i = 0; i < 3; i++){		tempPart.pos[i] += mtRandd(pimpl_->dr_, true);		if(tempPart.pos[i] > 1.0) tempPart.pos[i] -= 1.0;		else if(tempPart.pos[i] < 0.0) tempPart.pos[i] += 1.0;	}		if(nPart_ > 1){ //If only one particle, we don't need to check for collisions		//Check for collisions with other particles		for(uint i = 0; i < nPart_; i++){			if(i != randPart){				bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, pimpl_->particles_[i], pimpl_->box_);				if(isCollision) return false;			}		}		//Check for collisions with other particles' images		for(uint i = 0; i < nPart_; i++){			if(i != randPart){				std::vector<clam::vec3i>::const_iterator itr;				Particle ghostPart(pimpl_->particles_[i]);				for(itr = pimpl_->eil_.begin(); itr < pimpl_->eil_.end(); itr++){					for(uint j = 0; j < 3; j++) ghostPart.pos[j] = pimpl_->particles_[i].pos[j] + double((*itr)[j]);					bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, ghostPart, pimpl_->box_);					if(isCollision) return false;				}			}		}	}	//No collisions, set particle position	for(uint i = 0; i < 3; i++) pimpl_->particles_[randPart].pos[i] = tempPart.pos[i];		return true;}bool Simulation::rotation(void){	uint nPart_ = pimpl_->nPart_;	uint randPart = mtRandi(nPart_);	Particle tempPart(pimpl_->particles_[randPart]);	quat_t q_rotation;		//////////////////////////////////////////////////////	/* Generate Random Unit Vector */	/* Ref:Sphere Point Picking, Wolfram */	double x1 = mtRandd(1.0, true);	double x2 = mtRandd(1.0, true);	double s1 = sqr_d(x1) + sqr_d(x2);	while (s1 > 1.0){		x1 = mtRandd(1.0, true);		x2 = mtRandd(1.0, true);		s1 = sqr_d(x1) + sqr_d(x2);	}	double s2 = sqrt(1.0 - s1);		/* Generate Random Angle */	double dtheta_ = pimpl_->dtheta_;	double angle = mtRandd(dtheta_ * dtheta_ * dtheta_, false);	angle = pow(angle, 1.0/3.0);	while(mtRandd(1.0, false) > sqr_d(sin(angle)/angle)){		angle = mtRandd(dtheta_ * dtheta_ * dtheta_, false);		angle = pow(angle, 1.0/3.0);	}	double sina = sin(angle * 0.5);	/* Assemble Random Quaternion */	q_rotation.el[0] = cos(0.5 * angle);	q_rotation.el[1] = 2.0 * x1 * s2 * sina;	q_rotation.el[2] = 2.0 * x2 * s2 * sina;	q_rotation.el[3] = (1.0 - 2.0 * s1) * sina;	//////////////////////////////////////////////////////		/* Rotate Vertices */	std::vector<clam::vec3d>::iterator vertItr;	for(vertItr = tempPart.vertices.begin(); vertItr < tempPart.vertices.end(); vertItr++){		quat_rot(q_rotation, *vertItr);	}		if(nPart_ == 1){ //If only one particle, only check SIL		std::vector<clam::vec3i>::const_iterator itr;		//Check for collisions with self images		Particle ghostPart(tempPart);		for(itr = pimpl_->sil_.begin(); itr < pimpl_->sil_.end(); itr++){			for(uint j = 0; j < 3; j++) ghostPart.pos[j] = tempPart.pos[j] + double((*itr)[j]);			bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, ghostPart, pimpl_->box_);			if(isCollision) return false;		}	}	else{		//Check for collisions with other particles		for(uint i = 0; i < nPart_; i++){			if(i != randPart){				bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, pimpl_->particles_[i], pimpl_->box_);				if(isCollision) return false;			}		}		//Check for collisions with self images		{//Using brackets so ghostPart goes out of scope			Particle ghostPart(tempPart);			std::vector<clam::vec3i>::const_iterator itr;			for(itr = pimpl_->sil_.begin(); itr < pimpl_->sil_.end(); itr++){				for(uint j = 0; j < 3; j++) ghostPart.pos[j] = tempPart.pos[j] + double((*itr)[j]);				bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, ghostPart, pimpl_->box_);				if(isCollision) return false;			}		}		//Check for collisions with other particle's images		for(uint i = 0; i < nPart_; i++){			if(i != randPart){				Particle ghostPart(pimpl_->particles_[i]);				std::vector<clam::vec3i>::const_iterator itr;				for(itr = pimpl_->eil_.begin(); itr < pimpl_->eil_.end(); itr++){					for(uint k = 0; k < 3; k++) ghostPart.pos[k] = pimpl_->particles_[i].pos[k] + double((*itr)[k]);					bool isCollision = pimpl_->collisionDetector_.isColliding(tempPart, ghostPart, pimpl_->box_);					if(isCollision) return false;				}			}		}	}	//No collisions, set particle vertices	pimpl_->particles_[randPart].vertices = tempPart.vertices;		return true;}bool Simulation::scaling(void){	uint nPart_ = pimpl_->nPart_;	Box tempBox(pimpl_->box_);	double oldVol = tempBox.Volume();	double radV = mtRandd(pimpl_->dv_, true);	if(radV < -oldVol) return false; //Reject negative Volume	double newVol = oldVol + radV;		double arg = -pimpl_->bp_ * radV + nPart_ * log( newVol / oldVol);	double acc = 0.0;	if(arg >= 0.0) acc = 1.1;	else acc = exp(arg);		std::vector<clam::vec3i> tempSil;	std::vector<clam::vec3i> tempEil;		if(mtRandd(1.0, false) <= acc){		double fraction = pow(newVol/oldVol, 1.0/3.0);		for(uint i = 0; i < 9; i++) tempBox[i] = tempBox[i] * fraction;		if(nPart_ == 1){ //If only one particle, only check SIL			pimpl_->createImageLists(tempBox, tempSil, tempEil);			std::vector<clam::vec3i>::const_iterator itr;			//Check for collisions with self images			Particle ghostPart(pimpl_->particles_[0]);			for(itr = tempSil.begin(); itr < tempSil.end(); itr++){				for(uint j = 0; j < 3; j++) ghostPart.pos[j] = pimpl_->particles_[0].pos[j] + double((*itr)[j]);				bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[0], ghostPart, tempBox);				if(isCollision) return false;			}		}		else{			//Check for collisions between each particle			for(uint i = 0; i < nPart_; i++){				for(uint j = i + 1; j < nPart_; j++){					bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], pimpl_->particles_[j], tempBox);					if(isCollision) return false;				}			}			pimpl_->createImageLists(tempBox, tempSil, tempEil);			//Check for collisions with each particle's self images			for(uint i = 0; i < nPart_; i++){				Particle ghostPart(pimpl_->particles_[i]);				std::vector<clam::vec3i>::const_iterator itr;				for(itr = tempSil.begin(); itr < tempSil.end(); itr++){					for(uint j = 0; j < 3; j++) ghostPart.pos[j] = pimpl_->particles_[i].pos[j] + double((*itr)[j]);					bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], ghostPart, tempBox);					if(isCollision) return false;				}			}			//Check for collisions with other particle's images			for(uint i = 0; i < nPart_; i++){				for(uint j = i + 1; j < nPart_; j++){					Particle ghostPart(pimpl_->particles_[j]);					std::vector<clam::vec3i>::const_iterator itr;					for(itr = tempEil.begin(); itr < tempEil.end(); itr++){						for(uint k = 0; k < 3; k++) ghostPart.pos[k] = pimpl_->particles_[j].pos[k] + double((*itr)[k]);						bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], ghostPart, tempBox);						if(isCollision) return false;					}				}			}		}		//No collisions, set particle position		for(uint i = 0; i < 9; i++) pimpl_->box_[i] = tempBox[i];		pimpl_->sil_ = tempSil;		pimpl_->eil_ = tempEil;		return true;	}	else return false;}bool Simulation::deformation(void){	uint nPart_ = pimpl_->nPart_;	static uint boxIndices[] = {0, 4, 8, 1, 2, 5, 1, 2, 5};	int ratmp = mtRandi(9);	int randIndex = boxIndices[ratmp];	double deformation = mtRandd(pimpl_->ds_, true);		std::vector<clam::vec3i> tempSil;	std::vector<clam::vec3i> tempEil;		double arg;	if(ratmp < 3){		double oldVol = pimpl_->box_.Volume();		double newVol = (oldVol / pimpl_->box_[randIndex]) * (pimpl_->box_[randIndex] + deformation);		arg = -pimpl_->bp_ * (newVol - oldVol) + nPart_ * log(newVol/oldVol);		if(newVol < 0.0) return false; //Reject negative Volume	}	else arg = 0.0;		double exparg;	if(arg >= 0.0) exparg = 1.1;	else exparg = exp(arg);		if(mtRandd(1.0, false) <= exparg){		Box tempBox(pimpl_->box_);		tempBox[randIndex] = tempBox[randIndex] + deformation;		if(nPart_ == 1){ //If only one particle, only check SIL			pimpl_->createImageLists(tempBox, tempSil, tempEil);			std::vector<clam::vec3i>::const_iterator itr;			//Check for collisions with self images			Particle ghostPart(pimpl_->particles_[0]);			for(itr = tempSil.begin(); itr < tempSil.end(); itr++){				for(uint j = 0; j < 3; j++) ghostPart.pos[j] = pimpl_->particles_[0].pos[j] + double((*itr)[j]);				bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[0], ghostPart, tempBox);				if(isCollision) return false;			}		}		else{			//Check for collisions between each particle			for(uint i = 0; i < nPart_; i++){				for(uint j = i + 1; j < nPart_; j++){					bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], pimpl_->particles_[j], tempBox);					if(isCollision) return false;				}			}			pimpl_->createImageLists(tempBox, tempSil, tempEil);			//Check for collisions with each particle's self images			for(uint i = 0; i < nPart_; i++){				Particle ghostPart(pimpl_->particles_[i]);				std::vector<clam::vec3i>::const_iterator itr;				for(itr = tempSil.begin(); itr < tempSil.end(); itr++){					for(uint j = 0; j < 3; j++) ghostPart.pos[j] = pimpl_->particles_[i].pos[j] + double((*itr)[j]);					bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], ghostPart, tempBox);					if(isCollision) return false;				}			}			//Check for collisions with other particle's images			for(uint i = 0; i < nPart_; i++){				for(uint j = i + 1; j < nPart_; j++){					Particle ghostPart(pimpl_->particles_[j]);					std::vector<clam::vec3i>::const_iterator itr;					for(itr = tempEil.begin(); itr < tempEil.end(); itr++){						for(uint k = 0; k < 3; k++) ghostPart.pos[k] = pimpl_->particles_[j].pos[k] + double((*itr)[k]);						bool isCollision = pimpl_->collisionDetector_.isColliding(pimpl_->particles_[i], ghostPart, tempBox);						if(isCollision) return false;					}				}			}		}		//No collisions, set particle position		pimpl_->box_[randIndex] = tempBox[randIndex];		pimpl_->sil_ = tempSil;		pimpl_->eil_ = tempEil;		return true;	}	return false;}void Simulation::Impl::createImageLists(Box box, std::vector<clam::vec3i> &sil, std::vector<clam::vec3i> &eil)const{	//Find the vertices of the limiting cube in reduced coordinates	if(!sil.empty()) sil.clear();	if(!eil.empty()) eil.clear();	std::vector<clam::vec3d> cubeVerts;	clam::mat3d boxInv = box.inverse();	for(int i = -1; i <= 2; i += 2){		for(int j = -1; j <= 2; j += 2){			for(int k = -1; k <= 2; k += 2){				clam::vec3d cubeVert(i, j, k);				clam::vec3d cubeVertRed;				cubeVertRed[0] = 2.0 * Rmax_ * clam::dot(boxInv.Row(0), cubeVert);				cubeVertRed[1] = 2.0 * Rmax_ * clam::dot(boxInv.Row(1), cubeVert);				cubeVertRed[2] = 2.0 * Rmax_ * clam::dot(boxInv.Row(2), cubeVert);				cubeVerts.push_back(cubeVertRed);			}		}	}	//Find the maxima	std::vector<clam::vec3d>::iterator itr;	clam::vec3d maxVec = cubeVerts[0];	for(itr = cubeVerts.begin() + 1; itr < cubeVerts.end(); itr++){		for(uint i = 0; i < 3; i++){			if((*itr)[i] > maxVec[i]) maxVec[i] = (*itr)[i];		}	}	clam::vec3i maxInts;	for(uint i = 0; i < 3; i++) maxInts[i] = int(ceil(maxVec[i]));	//Create the SIL	clam::vec3i one(1, 1, 1);	for(int i = -maxInts[0]; i <= maxInts[0]; i++){		for(int j = -maxInts[1]; j <= maxInts[1]; j++){			for(int k = -maxInts[2]; k <= maxInts[2]; k++){				if(i == 0 && j == 0 && k == 0) continue;				clam::vec3i point(i, j, k);				sil.push_back(point);			}		}	}	//Create the EIL if more than one particle	if(nPart_ > 1){		eil = sil;		for(int m = 0; m < 3; m++){			for(int i = -maxInts[m]; i <= maxInts[m]; i++){					for(int j = -maxInts[(m + 1)%3]; j <= maxInts[(m + 1)%3] /*+ 1*/; j++){ //Corners are excluded. Add them back if there are problems					clam::vec3i point;					point[m] = i;					point[(m + 1)%3] = j;					point[(m + 2)%3] = maxInts[(m + 2)%3] + 1;					eil.push_back(point);				}			}		}		//eil.push_back(clam::vec3i(maxInts[0]+1, maxInts[1]+1, maxInts[2]+1));	}}void Simulation::Impl::triad(Particle p, double* aa)const{	////////////////////////////////////////////////////	// Find the Axis-Angle Representation using the   //	// Triad method of attitude estimation.			  //	// Algorithm for Angle-Axis conversion can be	  //	// found in "Rotation: ..." by Rebecca M. Brannon //	////////////////////////////////////////////////////	uint TriadV1 = types_[p.type].triadV1;	uint TriadV2 = types_[p.type].triadV2;	double a[3], b[3], c[3], A[3], B[3], C[3], R[9];	double norm_a = 0.0, norm_b = 0.0, norm_c = 0.0;	double norm_A = 0.0, norm_B = 0.0, norm_C = 0.0;		/* Get Orthonormal Basis */		b[0] = 		types_[p.type].vertices[TriadV1][1] * types_[p.type].vertices[TriadV2][2] -		types_[p.type].vertices[TriadV1][2] * types_[p.type].vertices[TriadV2][1];	b[1] =		types_[p.type].vertices[TriadV1][2] * types_[p.type].vertices[TriadV2][0] -		types_[p.type].vertices[TriadV1][0] * types_[p.type].vertices[TriadV2][2];	b[2] = 		types_[p.type].vertices[TriadV1][0] * types_[p.type].vertices[TriadV2][1] -		types_[p.type].vertices[TriadV1][1] * types_[p.type].vertices[TriadV2][0];		c[0] = types_[p.type].vertices[TriadV1][1] * b[2] - types_[p.type].vertices[TriadV1][2] * b[1];	c[1] = types_[p.type].vertices[TriadV1][2] * b[0] - types_[p.type].vertices[TriadV1][0] * b[2];	c[2] = types_[p.type].vertices[TriadV1][0] * b[1] - types_[p.type].vertices[TriadV1][1] * b[0];		B[0] = p.vertices[TriadV1][1] * p.vertices[TriadV2][2] - p.vertices[TriadV1][2] * p.vertices[TriadV2][1];	B[1] = p.vertices[TriadV1][2] * p.vertices[TriadV2][0] - p.vertices[TriadV1][0] * p.vertices[TriadV2][2];	B[2] = p.vertices[TriadV1][0] * p.vertices[TriadV2][1] - p.vertices[TriadV1][1] * p.vertices[TriadV2][0];		C[0] = p.vertices[TriadV1][1] * B[2] - p.vertices[TriadV1][2] * B[1];	C[1] = p.vertices[TriadV1][2] * B[0] - p.vertices[TriadV1][0] * B[2];	C[2] = p.vertices[TriadV1][0] * B[1] - p.vertices[TriadV1][1] * B[0];		for(uint i = 0; i < 3; i++){		norm_a += sqr_d(types_[p.type].vertices[TriadV1][i]);		norm_b += sqr_d(b[i]);		norm_c += sqr_d(c[i]);		norm_A += sqr_d(p.vertices[TriadV1][i]);		norm_B += sqr_d(B[i]);		norm_C += sqr_d(C[i]);	}	norm_a = sqrt(norm_a);	norm_b = sqrt(norm_b);	norm_c = sqrt(norm_c);	norm_A = sqrt(norm_A);	norm_B = sqrt(norm_B);	norm_C = sqrt(norm_C);		for(uint i = 0; i < 3; i++){		a[i] = types_[p.type].vertices[TriadV1][i] / norm_a;		b[i] /= norm_b;		c[i] /= norm_c;		A[i] = p.vertices[TriadV1][i] / norm_A;		B[i] /= norm_B;		C[i] /= norm_C;	}		/* Calculate the Rotation Matrix */		for(uint i = 0; i < 3; i++){		for(uint j = 0; j < 3; j++) R[3*i+j] = A[i] * a[j] + B[i] * b[j] + C[i] * c[j];	}	/* Convert Rotation Matrix to Angle-Axis Representation */	double t = R[0] + R[4] + R[8];	double cos_a = 0.5 * (t - 1.0);	double diff_cos = sqrt(1.0 - sqr_d(cos_a));	aa[0] = acos(cos_a);	if(diff_cos > 0.1e-10){		double ss = 1.0 / (2.0 * sin(aa[0]));		aa[1] = ss * (R[7] - R[5]);		aa[2] = ss * (R[2] - R[6]);		aa[3] = ss * (R[3] - R[1]);	}	else if(cos_a > 0.0){		aa[1] = 1.0;		aa[2] = 0.0;		aa[3] = 0.0;	}	else{		uint column = 0;		double norm;		for(uint i = 0; i < 3; i++){			norm = 0.0;			uint flag = 0;			for(uint j = 0; j < 3; j++){				if(R[3*j+i] == 0.0) flag += 1;				else norm += sqr_d(R[3*j+i]);			}			if(flag < 3){				column = i;				break;			}		}		norm = sqrt(norm);				aa[0] = M_PI;		aa[1] = R[column] / norm;		aa[2] = R[3+column] / norm;		aa[3] = R[6+column] / norm;			}	aa[0] = aa[0] * 180.0 / M_PI;}bool Simulation::printConfig(std::ostream &outStream)const{	if(!outStream.good()){		std::cout << "The output stream went wrong." << std::endl;		return false;	}		outStream << pimpl_->nPart_ << std::endl;	outStream << pimpl_->types_.size() << std::endl;		outStream.setf(std::ios::fixed, std::ios::floatfield);	outStream.precision(6);		for(uint i = 0; i < 9; i++){		outStream << pimpl_->box_[i] << "\t";	}	outStream << std::endl;		for(uint i = 0; i < pimpl_->nPart_; i++){		outStream << pimpl_->particles_[i].type << "\t";		for(uint j = 0; j < 3; j++)	outStream << clam::dot(pimpl_->box_.Row(j), pimpl_->particles_[i].pos) << "\t";		double Rot[4] = {0.0};		if(pimpl_->types_[pimpl_->particles_[i].type].name != "Sphere") pimpl_->triad(pimpl_->particles_[i], Rot);		else Rot[1] = 1.0;		for(uint j = 0; j < 4; j++) outStream << Rot[j] << "\t";		outStream << std::endl;	}		for(uint i = 0; i < pimpl_->types_.size(); i++){		outStream << pimpl_->types_[i].name;		if( pimpl_->types_[i].scale != 1.0 ) outStream << "\t" << pimpl_->types_[i].scale;		outStream << std::endl;	}		return true;}bool Simulation::readConfig(std::istream &inStream){	uint nTypes;	std::vector<quat_t> rotations;		if(!inStream.good()){		std::cout << "The input stream went bad." << std::endl;		return false;	}		std::cout << "Parsing Configuration from provided input stream." << std::endl;		std::string line;	std::getline(inStream, line);	std::istringstream s(line);		//Read number of particles	s >> pimpl_->nPart_;	std::cout << "Found " << pimpl_->nPart_ << " particles." << std::endl;		s.clear();	std::getline(inStream, line);	s.str(line);		//Read number of particle types	s >> nTypes;		s.clear();	std::getline(inStream, line);	s.str(line);		//Read Box properties	for(uint i = 0; i < 9; i++) s >> pimpl_->box_[i];	clam::mat3d boxInverse(pimpl_->box_.inverse());		//Read Particle position, type and rotation	for(uint i = 0; i < pimpl_->nPart_; i++){		std::getline(inStream, line);		std::istringstream stream(line);		Particle tempParticle;		quat_t tempRot;		stream >> tempParticle.type >> std::ws;		stream >> tempParticle.pos[0] >> tempParticle.pos[1] >> tempParticle.pos[2] >> std::ws;		for(uint j = 0; j < 3; j++) tempParticle.pos[j] = clam::dot(boxInverse.Row(j), tempParticle.pos);		stream >> tempRot.el[0] >> tempRot.el[1] >> tempRot.el[2] >> tempRot.el[3] >> std::ws;		//stream >> tempParticle.scale >> std::ws; //In the future		rotations.push_back(tempRot);		pimpl_->particles_.push_back(tempParticle);	}		//Read type names and initiate types	for(uint i = 0; i < nTypes; i++){		std::getline(inStream, line);		std::istringstream stream(line);		std::string typeName;		double scale = 1.0;		stream >> typeName >> std::ws;		if(!stream.eof()) stream >> scale >> std::ws;		Ptype tempType;		if(typeName != "Sphere"){			if(!readProperties(typeName, tempType, scale)) return false;		}		else tempType.name = "Sphere";		pimpl_->types_.push_back(tempType);	}		//Rotate each particle's vertices acording to the rotations read from the input inStream	//Also find the maximum circumscribed radius or just radius in case of spheres	double max = 0.0;	for(uint i = 0; i < pimpl_->nPart_; i++){		uint partType = pimpl_->particles_[i].type;		if(pimpl_->types_[partType].name != "Sphere"){			pimpl_->particles_[i].vertices = pimpl_->types_[partType].vertices;			quat_t q_rotation;			rotations[i].el[0] *= M_PI / 180.0;			q_rotation.el[0] = cos(0.5 * rotations[i].el[0]);			q_rotation.el[1] = rotations[i].el[1] * sin(0.5 * rotations[i].el[0]);			q_rotation.el[2] = rotations[i].el[2] * sin(0.5 * rotations[i].el[0]);			q_rotation.el[3] = rotations[i].el[3] * sin(0.5 * rotations[i].el[0]);			std::vector<clam::vec3d>::iterator itr;			for(itr = pimpl_->particles_[i].vertices.begin(); itr < pimpl_->particles_[i].vertices.end(); itr++){				quat_rot(q_rotation, *itr);			}			if(pimpl_->particles_[i].scale * pimpl_->types_[partType].cscrb > max) max = pimpl_->types_[partType].cscrb;		}		else if(pimpl_->particles_[i].scale > max) max = pimpl_->particles_[i].scale;	}	pimpl_->Rmax_ = max;	std::cout << "Successfully done parsing." << std::endl;	std::cout << std::endl;	pimpl_->collisionDetector_.Init(pimpl_->types_);	pimpl_->createImageLists(pimpl_->box_, pimpl_->sil_, pimpl_->eil_);	return true;}static bool readProperties(std::string typeName, Ptype &ptype, double scale = 1.0){	std::cout << "Parsing Properties for " << typeName << "." << std::endl;	ptype.name = typeName;	ptype.scale = scale;	std::vector<clam::vec3d> vertices;	std::vector< std::vector<uint> > faces;	std::vector< std::vector<uint> > edges;	std::vector< std::vector<uint> > vertNeigh;		/* Parse File */	const char *filepath = ("obj/" + typeName + ".obj").c_str();	std::ifstream file(filepath);	if(!file){		std::cout << "Error parsing file \"" << filepath << "\" by function " << "'" << __FUNCTION__ << "'" << std::endl;		return false;	}		std::string line;	while(std::getline(file, line)){		if(line[0] == 'v'){			std::istringstream s(line.substr(2));			clam::vec3d vertex;			s >> vertex[0] >> vertex[1] >> vertex[2] >> std::ws;			for(int i = 0; i < 3; i++) vertex[i] *= scale;			vertices.push_back(vertex);		}		else if(line[0] == 'f'){			std::istringstream s(line.substr(2));			std::vector<uint> face;			uint faceVertexIndex;			while(s >> faceVertexIndex) face.push_back(faceVertexIndex - 1);			faces.push_back(face);		}	}	file.close();	ptype.vertices = vertices;	std::cout << "Found " << vertices.size() << " vertices." << std::endl;		/* Calculate Properties */		//Find Edges	std::vector< std::vector<uint> >::const_iterator faceItr1, faceItr2;	//Iterate over each face and for each next face in the list, check if they 	//share two vertices, this defines an edge.	for(faceItr1 = faces.begin(); faceItr1 < faces.end(); faceItr1++){		for(faceItr2 = faceItr1 + 1; faceItr2 < faces.end(); faceItr2++){			uint fcount = 0;			std::vector<uint> edge;			std::vector<uint>::const_iterator itr1, itr2;			for(itr1 = (*faceItr1).begin(); itr1 < (*faceItr1).end(); itr1++){				for(itr2 = (*faceItr2).begin(); itr2 < (*faceItr2).end(); itr2++){					if(*itr1 == *itr2){						edge.push_back(*itr1);						fcount++;					}				}				if(fcount == 2){					edges.push_back(edge);					fcount = 0;					edge.clear();				}			}		}	}	std::cout << "Found " << edges.size() << " edges." << std::endl;		//Find Vertex Neighbours	//For all vertices, check if two edges share this vertex. If they do and it	//isn't vertex 0, append the other vertices of these edge to the neighbor list	for(uint vi = 0; vi < vertices.size(); vi++){		std::vector<uint> neighbors;		for(uint ei = 0; ei < edges.size(); ei++){			for(uint i = 0; i < 2; i++){				if(edges[ei][i] == vi){					if(edges[ei][(i+1)%2] != 0) neighbors.push_back(edges[ei][(i+1)%2]);				}			}		}		if(!neighbors.empty()) vertNeigh.push_back(neighbors);	}	ptype.vertNeigh = vertNeigh;		//Find the inRadius	//For each face, calculate its distance from the particle's center and find the min	std::vector< std::vector<uint> >::const_iterator faceItr;	double minDistance = 100.0;	for(faceItr = faces.begin(); faceItr < faces.end(); faceItr++){		clam::vec3d p(vertices[(*faceItr)[0]]);				clam::vec3d a(			vertices[(*faceItr)[1]][0] - vertices[(*faceItr)[0]][0],			vertices[(*faceItr)[1]][1] - vertices[(*faceItr)[0]][1],			vertices[(*faceItr)[1]][2] - vertices[(*faceItr)[0]][2]		);				clam::vec3d b(			vertices[(*faceItr)[2]][0] - vertices[(*faceItr)[0]][0],			vertices[(*faceItr)[2]][1] - vertices[(*faceItr)[0]][1],			vertices[(*faceItr)[2]][2] - vertices[(*faceItr)[0]][2]		);				clam::vec3d normal = a.cross(b);		double length = clam::length(normal);		for(int i = 0; i < 3; i++) normal[i] /= length;		double faceDistance = fabs(clam::dot(normal, p));				if(faceDistance < minDistance) minDistance = faceDistance;	}	ptype.iscrb = minDistance;	std::cout << "Inscribed Radius: " << ptype.iscrb << std::endl;		//Find the circumRadius	//It's just the farthest vertex from the particle's center	double maxDistance = 0.0;	std::vector<clam::vec3d>::const_iterator vItr;	for(vItr = vertices.begin(); vItr < vertices.end(); vItr++){		double vertexLength = clam::length(*vItr);		if( vertexLength > maxDistance) maxDistance = vertexLength;	}	ptype.cscrb = maxDistance;	std::cout << "Circumscribed Radius: " << ptype.cscrb << std::endl;		//Find two linearly independent vertices	ptype.triadV1 = 1;	for(uint i = 1; i < vertices.size(); i++){		double crossLength = clam::length(vertices[ptype.triadV1].cross(vertices[i]));		if(crossLength > 1.0e-10){			ptype.triadV2 = i;			break;		}	}	std::cout << "Triad V1: " << ptype.triadV1 << ", Triad V2: " << ptype.triadV2 << std::endl;	std::cout << std::endl;		return true;}}