#ifndef SIMULATION_BASE_H
#define SIMULATION_BASE_H

#ifdef __WIN32
	#ifdef DLL_EXPORT
		#define FBMCAPI __declspec(dllexport)
	#else
		#define FBMCAPI __declspec(dllimport)
	#endif
#else
	#define FBMCAPI
#endif

#include <memory>

namespace fbmc{

	enum eProperties{
		MaxTranslation,
		MaxRotation,
		MaxVolume,
		MaxDeformation,
		Pressure
	};

	class FBMCAPI Simulation{
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
		void scaleBox(double); //only factors > 1
		
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