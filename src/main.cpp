#include <iostream>
#include "../include/simulation_base.h"


class Simulation: public simulationBase{
public:
	void loop(void){
		std::string filename = "test.dat";
		bp_ = 0.2;
		for(mcStep_ = 0; mcStep_ < 50000; mcStep_++){
			scaling();
			rotation();
			translation();
			deformation();
			bp_+=0.002;
		}
		
		// std::vector<clam::vec3i>::iterator itr;
		// for(itr = sil_.begin(); itr < sil_.end(); itr++){
			// std::cout << (*itr)[0] << ", " << (*itr)[1] << ", " << (*itr)[2] << std::endl;
		// }
		// std::cout << "SIL size: " << sil_.size() << std::endl;
		// std::cout << std::endl;
		
		// for(itr = eil_.begin(); itr < eil_.end(); itr++){
			// std::cout << (*itr)[0] << ", " << (*itr)[1] << ", " << (*itr)[2] << std::endl;
		// }
		// std::cout << "EIL size: " << eil_.size() << std::endl;
		
		printConfig(filename);
		std::cout << "Final Volume: " << box_.Volume() << std::endl;
	}
private:
	uint mcStep_;
};

int main(int argc, char *argv[]){
	Simulation TestSim;
	if(TestSim.Init(argc, argv)) TestSim.loop();
	
	return 0;
}