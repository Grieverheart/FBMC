#include "../include/clam.h"
#include <math.h>

namespace clam{

	////////////////////////////////Vec3f//////////////////////////////////////////
	vec3d::vec3d(double x, double y, double z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	vec3d vec3d::cross(vec3d const &other_v)const{
		vec3d cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	double dot(vec3d const &a, vec3d const &b){
		double result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	double length(vec3d const &a){
		return sqrt(dot(a, a));
	}

	double vec3d::operator[](unsigned int i)const{
		return data_[i];
	}

	double& vec3d::operator[](unsigned int i){
		return data_[i];
	}
	
	////////////////////////////////Vec3d//////////////////////////////////////////
	vec3f::vec3f(float x, float y, float z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	vec3f vec3f::cross(vec3f const &other_v)const{
		vec3f cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	float dot(vec3f const &a, vec3f const &b){
		float result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	float length(vec3f const &a){
		return sqrt(dot(a, a));
	}

	float vec3f::operator[](unsigned int i)const{
		return data_[i];
	}

	float& vec3f::operator[](unsigned int i){
		return data_[i];
	}
	
	////////////////////////////////Vec3i//////////////////////////////////////////
	vec3i::vec3i(int x, int y, int z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	vec3i vec3i::cross(vec3i const &other_v)const{
		vec3i this_v = *this;
		vec3i cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	int dot(vec3i const &a, vec3i const &b){
		int result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	int length(vec3i const &a){
		return int(sqrt(dot(a, a)));
	}

	int vec3i::operator[](unsigned int i)const{
		return data_[i];
	}

	int& vec3i::operator[](unsigned int i){
		return data_[i];
	}
	
	////////////////////////////////Mat3d//////////////////////////////////////////
	mat3d::mat3d(double const x){
		for(int i = 0; i < 9; i++) data_[i] = 0.0;
		for(int i = 0; i < 3; i++) data_[3*i+i] = x;
	}
	
	mat3d::mat3d(vec3d const &x, vec3d const &y, vec3d const &z){
		data_[0] = x[0];
		data_[1] = y[0];
		data_[2] = z[0];
		data_[3] = x[1];
		data_[4] = y[1];
		data_[5] = z[1];
		data_[6] = x[2];
		data_[7] = y[2];
		data_[8] = z[2];
	}

	vec3d mat3d::Col(unsigned int i)const{
		return vec3d(data_[3*i+0], data_[3*i+1], data_[3*i+2]);
	}
	
	mat3d operator*(mat3d const &a, mat3d const &b){
		mat3d result;
		
		for(int i = 0; i < 3; i++){
			for(int k = 0; k < 3; k++){
				for(int j = 0; j < 3; j++){
					result[3*k+i] += a[3*j+i] * b[3*k+j];   
				}
			}
		}
		
		return result;
	}
	
	double mat3d::operator[](unsigned int i)const{
		return data_[i];
	}
	
	double& mat3d::operator[](unsigned int i){
		return data_[i];
	}
	
	////////////////////////////////Mat3f//////////////////////////////////////////
	mat3f::mat3f(float const x){
		for(int i = 0; i < 9; i++) data_[i] = 0.0;
		for(int i = 0; i < 3; i++) data_[3*i+i] = x;
	}
	
	mat3f::mat3f(vec3f const &x, vec3f const &y, vec3f const &z){
		data_[0] = x[0];
		data_[1] = y[0];
		data_[2] = z[0];
		data_[3] = x[1];
		data_[4] = y[1];
		data_[5] = z[1];
		data_[6] = x[2];
		data_[7] = y[2];
		data_[8] = z[2];
	}

	vec3f mat3f::Col(unsigned int i)const{
		return vec3f(data_[3*i+0], data_[3*i+1], data_[3*i+2]);
	}
	
	mat3f operator*(mat3f const &a, mat3f const &b){
		mat3f result;
		
		for(int i = 0; i < 3; i++){
			for(int k = 0; k < 3; k++){
				for(int j = 0; j < 3; j++){
					result[3*k+i] += a[3*j+i] * b[3*k+j];   
				}
			}
		}
		
		return result;
	}
	
	float mat3f::operator[](unsigned int i)const{
		return data_[i];
	}
	
	float& mat3f::operator[](unsigned int i){
		return data_[i];
	}
}