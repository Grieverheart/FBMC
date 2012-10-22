#include <iostream>
#include "../include/simulation_base.h"


class Simulation: public simulationBase{
public:
	void loop(void){
		std::string filename = "test.dat";
		
		std::vector<clam::vec3i>::iterator itr;
		for(itr = sil_.begin(); itr < sil_.end(); itr++){
			std::cout << (*itr)[0] << ", " << (*itr)[1] << ", " << (*itr)[2] << std::endl;
		}
		std::cout << "SIL size: " << sil_.size() << std::endl;
		std::cout << std::endl;
		
		for(itr = eil_.begin(); itr < eil_.end(); itr++){
			std::cout << (*itr)[0] << ", " << (*itr)[1] << ", " << (*itr)[2] << std::endl;
		}
		std::cout << "EIL size: " << eil_.size() << std::endl;
		
		saveConfig(filename);
	}
private:
	uint mcStep_;
};

int main(int argc, char *argv[]){
	Simulation TestSim;
	if(TestSim.Init(argc, argv)) TestSim.loop();
	
	return 0;
}