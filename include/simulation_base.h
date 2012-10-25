#ifndef SIMULATION_BASE_H
#define SIMULATION_BASE_H

#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include "box.h"
#include "particle.h"
#include "ptype.h"
#include "collisionDetection.h"
#include "typedefs.h"

class simulationBase{
public:
	simulationBase(void){};
	virtual ~simulationBase(void){};
	bool Init(int argc, char *argv[]);
	virtual void loop(void)=0;

protected:
	double mtRandd(double x, bool isSymmetric)const;
	int mtRandi(int x);
	bool printConfig(std::string filename)const;
	void latticeReduction(double);
	
	bool translation(void);
	bool rotation(void);
	bool scaling(void);
	bool deformation(void);

//private:	
	bool readConfig(const char *filepath);
	void triad(Particle p, double *aa)const;
	void createImageLists(Box box, std::vector<clam::vec3i> &sil, std::vector<clam::vec3i> &eil)const;
	
	Box box_;
	std::vector<clam::vec3i> sil_;
	std::vector<clam::vec3i> eil_;
	std::vector<Particle> particles_;
	std::vector<Ptype> types_;
	uint nPart_;
	double dr_, dtheta_, dv_, ds_, bp_;
	double Rmax_; //Maximum double radius
	boost::random::mt19937 randGen_;
	CollisionDetector collisionDetector_;
};

#endif