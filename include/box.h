#ifndef SIM_BOX_H#define SIM_BOX_H#include "clam.h"class Box: public clam::mat3d{public:	Box(void):		clam::mat3d(){};	Box(double const x):		clam::mat3d(x){};	Box(clam::vec3d const &x, clam::vec3d const &y, clam::vec3d const &z):		clam::mat3d(x, y, z){};	~Box(void){};	double Area(void)const;	double Volume(void)const;};inline double Box::Area(void)const{	double area = 0.0;	Box temp = *this;	area += clam::length(temp.Col(0).cross(temp.Col(1)));	area += clam::length(temp.Col(0).cross(temp.Col(2)));	area += clam::length(temp.Col(1).cross(temp.Col(2)));	return 2.0*area;}inline double Box::Volume(void)const{	double volume = fabs(clam::dot((*this).Col(0), (*this).Col(1).cross((*this).Col(2))));	//double volume = data_[0] * data_[4] * data_[8];	return volume;}#endif