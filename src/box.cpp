#include "../include/box.h"

double Box::Area(void)const{
	double area = 0.0;
	Box temp = *this;
	area += clam::length(temp.Col(0).cross(temp.Col(1)));
	area += clam::length(temp.Col(0).cross(temp.Col(2)));
	area += clam::length(temp.Col(1).cross(temp.Col(2)));
	return area;
}

double Box::Volume(void)const{
	double volume = data_[0] * data_[4] * data_[8];
	return volume;
}