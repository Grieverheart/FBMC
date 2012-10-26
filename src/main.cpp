#include <iostream>
#include <fstream>
#include "../include/simulation_base.h"

typedef unsigned int uint;

int main(int argc, char *argv[]){
	fbmc::Simulation simulation;
	if(!simulation.Init(argc, argv)) return 0;
	
	for(uint i = 0; i < 10000; i++){
		simulation.translation();
		simulation.rotation();
		simulation.scaling();
		simulation.deformation();
	}
	
	
	std::cout << "Volume " << simulation.getVolume() << "." << std::endl;
	std::ofstream file("Data/test.dat");
	simulation.printConfig(file);
	file.close();
	
	return 0;
}