#ifndef SIMULATION_BASE_H
#define SIMULATION_BASE_H

#include <vector>
#include "box.h"
#include "particle.h"
#include "ptype.h"

#define DSFMT_MEXP 19937
#define HAVE_SSE2

extern "C"{
#include "dSFMT.h"
}

class simulationBase{
public:
	simulationBase(void){};
	virtual ~simulationBase(void)=0;
	void Init(int argc, char *argv[]);
	virtual void loop(void)=0;

protected:
	double mtRandd(double x, bool isSymmetric)const;
	int mtRandi(int x)const;
	
	virtual void saveConfig(void)=0;
	virtual void translation(void)=0;
	virtual void rotation(void)=0;
	virtual void scaling(void)=0;
	virtual void deformation(void)=0;
	void latticeReduction(double);
	
	Box box_;
	std::vector<clam::vec3i> sil_;
	std::vector<clam::vec3i> eil_;
	std::vector<Particle> particles_;
	std::vector<Ptype> types;
	unsigned int nPart;
	double dr_, dtheta_, dv;
	double Rmax_;
	dsfmt_t dsfmt_;
};

#endif