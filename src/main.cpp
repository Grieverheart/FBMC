#include <iostream>
#include "../include/simulation_base.h"


class Simulation: public simulationBase{
public:
	void loop(void){
	}
private:
	unsigned int mcStep_;
};

int main(int argc, char *argv[]){
	Simulation TestSim;
	if(TestSim.Init(argc, argv)) TestSim.loop();
	
	return 0;
}