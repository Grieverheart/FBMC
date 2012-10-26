#ifndef SIMULATION_BASE_H
#define SIMULATION_BASE_H

#include <memory>

namespace fbmc{

	enum eProperties{
		MaxTranslation,
		MaxRotation,
		MaxVolume,
		MaxDeformation,
		Pressure,
	};

	class Simulation{
	public:
		Simulation(void);
		virtual ~Simulation(void);
		bool Init(int argc, char *argv[]);
		
		//Simulation Moves
		bool translation(void);
		bool rotation(void);
		bool scaling(void);
		bool deformation(void);
		void latticeReduction(double);
		
		//Utility functions
		double mtRandd(double x, bool isSymmetric)const;
		int mtRandi(int x);
		bool printConfig(std::ostream &)const;
		bool readConfig(std::istream &);
		double getVolume(void)const;
		double getProperty(eProperties property)const;
		void setProperty(eProperties property, double value);

	private:
		struct Impl;
		std::unique_ptr<Impl> pimpl_;
	};
}

#endif